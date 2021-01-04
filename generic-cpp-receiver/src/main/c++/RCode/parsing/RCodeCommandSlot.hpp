/*
 * RCodeCommandSlot.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSLOT_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSLOT_HPP_
#include "../RCodeIncludes.hpp"

#include "RCodeBigField.hpp"
#include "RCodeFieldMap.hpp"
#include "../RCodeDebugOutput.hpp"
#include "instreams/RCodeLookaheadStream.hpp"
#include "instreams/RCodeMarkerInStream.hpp"

template<class RP>
class RCode;

template<class RP>
class RCodeCommand;

template<class RP>
class RCodeCommandInStream;

template<class RP>
class RCodeCommandSequence;

template<class RP>
class RCodeParser;

struct RCodeCommandSlotStatus {
    bool parsed :1;
    bool complete :1;
    bool needsMoveAlong :1;
    bool started :1;
    bool usesBigBig :1;
    bool hasCheckedCommand :1;

    void reset() {
        parsed = false;
        complete = false;
        needsMoveAlong = false;
        started = false;
        usesBigBig = false;
        hasCheckedCommand = false;
    }
};

template<class RP>
class RCodeCommandSlot {
    typedef typename RP::fieldUnit_t fieldUnit_t;
public:
    RCodeCommandSlot<RP> *next = NULL;
private:
    RCodeBigBigField<RP> *bigBig = NULL;
    RCodeCommand<RP> *cmd = NULL;
    char const *errorMessage = "";
    RCodeStandardBigField<RP> big;
    RCodeFieldMap<RP> map;
    RCodeResponseStatus status = OK;
    char end = 0;
    RCodeCommandSlotStatus slotStatus;

    uint8_t getHex(char c) {
        return (uint8_t)(c >= 'a' ? c - 'a' + 10 : c - '0');
    }

    bool isHex(char c) {
        return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
    }

    bool parseHexField(RCodeCommandInStream<RP> *in, char field);

    void failParse(RCodeCommandInStream<RP> *in, RCodeCommandSequence<RP> *sequence, RCodeResponseStatus errorStatus, char const *errorMessage);

public:
    RCodeCommandSlot() {
        slotStatus.reset();
    }

    void setup(RCodeBigBigField<RP> *bigBig) {
        this->bigBig = bigBig;
    }

    void reset() {
        next = NULL;
        map.reset();
        big.reset();
        status = OK;
        slotStatus.reset();
        if (RP::bigBigFieldLength > 0 && slotStatus.usesBigBig) {
            bigBig->reset();
        }
        cmd = NULL;
    }

    void start() {
        slotStatus.started = true;
    }
    bool isStarted() const {
        return slotStatus.started;
    }
    void terminate() {
        end = '\n';
    }
    bool isComplete() const {
        return slotStatus.complete;
    }
    void setComplete(bool complete) {
        this->slotStatus.complete = complete;
    }
    bool needsMoveAlong() const {
        return slotStatus.needsMoveAlong;
    }
    void setNeedsMoveAlong(bool needsMoveAlong) {
        this->slotStatus.needsMoveAlong = needsMoveAlong;
    }
    RCodeFieldMap<RP>* getFields() {
        return &map;
    }
    RCodeBigField<RP>* getBigField() {
        if (RP::bigBigFieldLength > 0 && slotStatus.usesBigBig) {
            return bigBig;
        }
        return &big;
    }
    bool isParsed() const {
        return slotStatus.parsed;
    }

    char getEnd() const {
        return end;
    }
    bool isEndOfSequence() const {
        return end == '\n';
    }

    RCodeResponseStatus getStatus() const {
        return status;
    }

    char const* getErrorMessage() const {
        return errorMessage;
    }

    RCodeCommand<RP>* getCommand(RCode<RP> *rcode);

    bool parseSingleCommand(RCodeCommandInStream<RP> *in, RCodeCommandSequence<RP> *sequence);

    void fail(const char *errorMessage, RCodeResponseStatus status) {
        this->errorMessage = errorMessage;
        this->status = status;
        this->end = '\n';
    }
};

template<class RP>
void RCodeCommandSlot<RP>::failParse(RCodeCommandInStream<RP> *in, RCodeCommandSequence<RP> *sequence, RCodeResponseStatus errorStatus,
        char const *errorMessage) {
    status = errorStatus;
    this->errorMessage = errorMessage;

    sequence->getRCode()->getDebug() << errorMessage << "\n";
    in->close();
    end = '\n';
}

template<class RP>
bool RCodeCommandSlot<RP>::parseHexField(RCodeCommandInStream<RP> *in, char field) {
    while (in->hasNext() && in->peek() == '0') {
        in->read();
    }
    RCodeLookaheadStream<RP> *l = in->getLookahead();
    char c = l->read();
    int lookahead = 0;
    while (isHex(c)) {
        c = l->read();
        lookahead++;
    }
    if (lookahead == 0) {
        return map.add(field, 0);
    } else if (lookahead == 1) {
        return map.add(field, getHex(in->read()));
    } else if (lookahead == 2) {
        return map.add(field, (uint8_t)(getHex(in->read()) << 4) | getHex(in->read()));
    } else {
        fieldUnit_t current = 0;
        for (int extra = lookahead % (sizeof(fieldUnit_t) * 2); extra > 0; extra--) {
            current = (fieldUnit_t)((current << 4) | getHex(in->read()));
            lookahead--;
        }
        if (current != 0 && !map.add(field, current)) {
            return false;
        }
        while (lookahead > 0) {
            for (uint8_t i = 0; i < sizeof(fieldUnit_t) * 2; i++) {
                current = (fieldUnit_t)((current << 4) | getHex(in->read()));
            }
            lookahead -= (int)(sizeof(fieldUnit_t) * 2);
            if (!map.add(field, current)) {
                return false;
            }
        }
        return true;

    }
}

template<class RP>
RCodeCommand<RP>* RCodeCommandSlot<RP>::getCommand(RCode<RP> *rcode) {
    if (cmd == NULL && !slotStatus.hasCheckedCommand) {
        slotStatus.hasCheckedCommand = true;
        cmd = rcode->getCommandFinder()->findCommand(this);
    }
    return cmd;
}

template<class RP>
bool RCodeCommandSlot<RP>::parseSingleCommand(RCodeCommandInStream<RP> *in, RCodeCommandSequence<RP> *sequence) {
    RCodeBigField<RP> *target = &big;
    in->open();
    reset();
    in->eatWhitespace();
    if (!in->hasNext()) {
        failParse(in, sequence, PARSE_ERROR, "No command present");
        return false;
    }
    char c;
    slotStatus.parsed = true;
    while (true) {
        if (in->hasNext()) {
            in->eatWhitespace();
        }
        if (in->hasNext()) {
            c = in->read();
            if (c >= 'A' && c <= 'Z') {
                if (map.has(c)) {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, PARSE_ERROR, "Same field appears twice");
                    return false;
                } else if (!parseHexField(in, c)) {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, TOO_BIG, "Too many fields");
                    return false;
                }
            } else if (c == '+') {
                if (target->getLength() != 0) {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, PARSE_ERROR, "Multiple big fields");
                    return false;
                }
                target->setIsString(false);
                uint8_t d = getHex(in->peek());

                while (in->hasNext() && isHex(in->peek())) {
                    d = (uint8_t)(getHex(in->read()) << 4);
                    if (!in->hasNext() || !isHex(in->peek())) {
                        slotStatus.hasCheckedCommand = true;
                        failParse(in, sequence, PARSE_ERROR, "Big field odd digits");
                        return false;
                    }
                    d = (uint8_t)(d + getHex(in->read()));
                    if (!target->addByteToBigField(d)) {
                        if (target == &big && RP::bigBigFieldLength > 0) {
                            slotStatus.usesBigBig = true;
                            target = bigBig;
                            bigBig->copyFrom(&big);
                            bigBig->addByteToBigField(d);
                        } else {
                            slotStatus.hasCheckedCommand = true;
                            failParse(in, sequence, TOO_BIG, "Big field too long");
                            return false;
                        }
                    }
                }
            } else if (c == '"') {
                if (target->getLength() != 0) {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, PARSE_ERROR, "Multiple big fields");
                    return false;
                }
                c = 0;
                target->setIsString(true);
                while (in->hasNext()) {
                    c = in->read();
                    if (c != '"') {
                        if (c == '\\') {
                            if (!in->hasNext()) {
                                break;
                            }
                            c = in->read();
                            if (c == 'n') {
                                c = '\n';
                            }
                        }
                        if (!target->addByteToBigField((uint8_t) c)) {
                            if (target == &big && RP::bigBigFieldLength > 0) {
                                slotStatus.usesBigBig = true;
                                target = bigBig;
                                bigBig->copyFrom(&big);
                                bigBig->addByteToBigField((uint8_t) c);
                            } else {
                                slotStatus.hasCheckedCommand = true;
                                failParse(in, sequence, TOO_BIG, "Big field too long");
                                return false;
                            }
                        }
                    } else {
                        break;
                    }
                }
                if (c != '"') {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, PARSE_ERROR, "Command sequence ended before end of big string field");
                    return false;
                }
            } else {
                slotStatus.hasCheckedCommand = true;
                sequence->getRCode()->getDebug() << "Unknown field marker: " << c << "\n";
                status = PARSE_ERROR;
                errorMessage = "Unknown field marker";
                in->close();
                end = '\n';
                return false;
            }
        } else {
            in->close();
            end = in->read();
            if (getCommand(sequence->getRCode()) == NULL) {
                failParse(in, sequence, UNKNOWN_CMD, "Command not known");
                return false;
            }
            return true;
        }
    }
}

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSLOT_HPP_ */

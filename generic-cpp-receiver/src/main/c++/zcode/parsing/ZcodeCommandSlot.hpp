/*
 * ZcodeCommandSlot.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDSLOT_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDSLOT_HPP_
#include "../ZcodeIncludes.hpp"

#include "ZcodeBigField.hpp"
#include "ZcodeFieldMap.hpp"
#include "../ZcodeDebugOutput.hpp"
#include "instreams/ZcodeLookaheadStream.hpp"
#include "instreams/ZcodeMarkerInStream.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeCommand;

template<class ZP>
class ZcodeCommandInStream;

template<class ZP>
class ZcodeCommandSequence;

template<class ZP>
class ZcodeParser;

struct ZcodeCommandSlotStatus {
        bool parsed :1;
        bool complete :1;
        bool needsMoveAlong :1;
        bool started :1;
        bool usesHuge :1;
        bool hasCheckedCommand :1;

        void reset() {
            parsed = false;
            complete = false;
            needsMoveAlong = false;
            started = false;
            usesHuge = false;
            hasCheckedCommand = false;
        }
};

template<class ZP>
class ZcodeCommandSlot {
        typedef typename ZP::fieldUnit_t fieldUnit_t;
        public:
        ZcodeCommandSlot<ZP> *next = NULL;
        private:
        ZcodeHugeField<ZP> *huge = NULL;
        ZcodeCommand<ZP> *cmd = NULL;
        char const *errorMessage = "";
        ZcodeStandardBigField<ZP> big;
        ZcodeFieldMap<ZP> map;
        ZcodeResponseStatus status = OK;
        char end = 0;
        ZcodeCommandSlotStatus slotStatus;

        uint8_t getHex(char c) {
            return (uint8_t) (c >= 'a' ? c - 'a' + 10 : c - '0');
        }

        bool isHex(char c) {
            return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f');
        }

        bool parseHexField(ZcodeCommandInStream<ZP> *in, char field);

        void failParse(ZcodeCommandInStream<ZP> *in, ZcodeCommandSequence<ZP> *sequence, ZcodeResponseStatus errorStatus, char const *errorMessage);

    public:
        ZcodeCommandSlot() {
            slotStatus.reset();
        }

        void setup(ZcodeHugeField<ZP> *huge) {
            this->huge = huge;
        }

        void reset() {
            next = NULL;
            map.reset();
            big.reset();
            status = OK;
            if (ZP::hugeFieldLength > 0 && slotStatus.usesHuge) {
                huge->reset();
            }
            slotStatus.reset();
            cmd = NULL;
        }

        void start() {
            slotStatus.started = true;
        }
        bool isStarted() const {
            return slotStatus.started;
        }
        void terminate() {
            end = Zchars::EOL_SYMBOL;
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
        ZcodeFieldMap<ZP>* getFields() {
            return &map;
        }
        ZcodeBigField<ZP>* getBigField() {
            if (ZP::hugeFieldLength > 0 && slotStatus.usesHuge) {
                return huge;
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
            return end == Zchars::EOL_SYMBOL;
        }

        ZcodeResponseStatus getStatus() const {
            return status;
        }

        char const* getErrorMessage() const {
            return errorMessage;
        }

        ZcodeCommand<ZP>* getCommand(Zcode<ZP> *zcode);

        bool parseSingleCommand(ZcodeCommandInStream<ZP> *in, ZcodeCommandSequence<ZP> *sequence);

        void fail(const char *errorMessage, ZcodeResponseStatus status) {
            this->errorMessage = errorMessage;
            this->status = status;
            this->end = Zchars::EOL_SYMBOL;
        }
};

template<class ZP>
void ZcodeCommandSlot<ZP>::failParse(ZcodeCommandInStream<ZP> *in, ZcodeCommandSequence<ZP> *sequence, ZcodeResponseStatus errorStatus,
        char const *errorMessage) {
    status = errorStatus;
    this->errorMessage = errorMessage;

    sequence->getZcode()->getDebug() << errorMessage << "\n";
    in->close();
    end = Zchars::EOL_SYMBOL;
}

template<class ZP>
bool ZcodeCommandSlot<ZP>::parseHexField(ZcodeCommandInStream<ZP> *in, char field) {
    while (in->hasNext() && in->peek() == '0') {
        in->read();
    }
    ZcodeLookaheadStream<ZP> *l = in->getLookahead();
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
        return map.add(field, (uint8_t) (getHex(in->read()) << 4) | getHex(in->read()));
    } else {
        fieldUnit_t current = 0;
        for (int extra = lookahead % (sizeof(fieldUnit_t) * 2); extra > 0; extra--) {
            current = (fieldUnit_t) ((current << 4) | getHex(in->read()));
            lookahead--;
        }
        if (current != 0 && !map.add(field, current)) {
            return false;
        }
        while (lookahead > 0) {
            for (uint8_t i = 0; i < sizeof(fieldUnit_t) * 2; i++) {
                current = (fieldUnit_t) ((current << 4) | getHex(in->read()));
            }
            lookahead -= (int) (sizeof(fieldUnit_t) * 2);
            if (!map.add(field, current)) {
                return false;
            }
        }
        return true;

    }
}

template<class ZP>
ZcodeCommand<ZP>* ZcodeCommandSlot<ZP>::getCommand(Zcode<ZP> *zcode) {
    if (cmd == NULL && !slotStatus.hasCheckedCommand) {
        slotStatus.hasCheckedCommand = true;
        cmd = zcode->getCommandFinder()->findCommand(this);
    }
    return cmd;
}

template<class ZP>
bool ZcodeCommandSlot<ZP>::parseSingleCommand(ZcodeCommandInStream<ZP> *in, ZcodeCommandSequence<ZP> *sequence) {
    ZcodeBigField<ZP> *target = &big;
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
            } else if (c == Zchars::BIGFIELD_PREFIX_MARKER) {
                if (target->getLength() != 0) {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, PARSE_ERROR, "Multiple big fields");
                    return false;
                }
                target->setIsString(false);
                uint8_t d = getHex(in->peek());

                while (in->hasNext() && isHex(in->peek())) {
                    d = (uint8_t) (getHex(in->read()) << 4);
                    if (!in->hasNext() || !isHex(in->peek())) {
                        slotStatus.hasCheckedCommand = true;
                        failParse(in, sequence, PARSE_ERROR, "Big field odd digits");
                        return false;
                    }
                    d = (uint8_t) (d + getHex(in->read()));
                    if (!target->addByteToBigField(d)) {
                        if (target == &big && ZP::hugeFieldLength > 0) {
                            slotStatus.usesHuge = true;
                            target = huge;
                            huge->copyFrom(&big);
                            huge->addByteToBigField(d);
                        } else {
                            slotStatus.hasCheckedCommand = true;
                            failParse(in, sequence, TOO_BIG, "Big field too long");
                            return false;
                        }
                    }
                }
            } else if (c == Zchars::BIGFIELD_QUOTE_MARKER) {
                if (target->getLength() != 0) {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, PARSE_ERROR, "Multiple big fields");
                    return false;
                }
                c = 0;
                target->setIsString(true);
                while (in->hasNext()) {
                    c = in->read();
                    if (c != Zchars::BIGFIELD_QUOTE_MARKER) {
                        if (c == Zchars::STRING_ESCAPE_SYMBOL) {
                            if (!in->hasNext()) {
                                break;
                            }
                            c = in->read();
                            if (c == 'n') {
                                c = Zchars::EOL_SYMBOL;
                            }
                        }
                        if (!target->addByteToBigField((uint8_t) c)) {
                            if (target == &big && ZP::hugeFieldLength > 0) {
                                slotStatus.usesHuge = true;
                                target = huge;
                                huge->copyFrom(&big);
                                huge->addByteToBigField((uint8_t) c);
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
                if (c != Zchars::BIGFIELD_QUOTE_MARKER) {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, PARSE_ERROR, "Command sequence ended before end of big string field");
                    return false;
                }
            } else {
                slotStatus.hasCheckedCommand = true;
                sequence->getZcode()->getDebug() << "Unknown field marker: " << c << "\n";
                status = PARSE_ERROR;
                errorMessage = "Unknown field marker";
                in->close();
                end = Zchars::EOL_SYMBOL;
                return false;
            }
        } else {
            in->close();
            end = in->read();
            if (getCommand(sequence->getZcode()) == NULL) {
                failParse(in, sequence, UNKNOWN_CMD, "Command not known");
                return false;
            }
            return true;
        }
    }
}

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDSLOT_HPP_ */

/*
 * RCodeCommandSlot.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#include "RCodeCommandSlotInterrim.hpp"

void RCodeCommandSlot::eatWhitespace(RCodeInStream *in) {
    char c = in->peek();
    while (in->hasNext() && (c == ' ' || c == '\t' || c == '\r')) {
        in->read();
        c = in->peek();
    }
}
bool RCodeCommandSlot::parseHexField(RCodeInStream *in, char field) {
    while (in->hasNext() && in->peek() == '0') {
        in->read();
    }
    RCodeLookaheadStream *l = in->getLookahead();
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
        return map.add(field, (getHex(in->read()) << 4) | getHex(in->read()));
    } else {
        fieldUnit current = 0;
        for (int extra = lookahead % (sizeof(fieldUnit) * 2); extra > 0;
                extra--) {
            current <<= 4;
            current += getHex(in->read());
            lookahead--;
        }
        if (current != 0 && !map.add(field, current)) {
            return false;
        }
        while (lookahead > 0) {
            for (int i = 0; i < 1 * 2; i++) {
                current <<= 4;
                current |= getHex(in->read());
            }
            lookahead -= 1 * 2;
            if (!map.add(field, current)) {
                return false;
            }
        }
        return true;

    }
}
RCodeCommand* RCodeCommandSlot::getCommand(RCode *rcode) {
    if (cmd == NULL) {
        cmd = rcode->getCommandFinder()->findCommand(this);
    }
    return cmd;
}
bool RCodeCommandSlot::parseSingleCommand(RCodeInStream *in,
        RCodeCommandSequence *sequence) {
    RCodeBigField *target = &big;
    in->openCommand();
    reset();
    char c;
    eatWhitespace(in);
    if (in->peek() == '#') {
        if (sequence->peekFirst() == NULL && !sequence->isRunning()) {
            status = SKIP_COMMAND;
            errorMessage = "# found, command sequence is comment";
            in->closeCommand();
            end = '\n';
            return false;
        } else {
            status = PARSE_ERROR;
            errorMessage = "# only valid on first command of sequence";
            in->closeCommand();
            end = '\n';
            return false;
        }
    }
    if (in->peek() == '*') {
        in->read();
        if (in->hasNext()) {
            eatWhitespace(in);
        }
        if (sequence->peekFirst() == NULL && !sequence->isRunning()) {
            sequence->setBroadcast();
        } else {
            status = PARSE_ERROR;
            errorMessage = "* only valid on first command of sequence";
            in->closeCommand();
            end = '\n';
            return false;
        }
    }
    if (in->peek() == '%') {
        in->read();
        if (in->hasNext()) {
            eatWhitespace(in);
        }
        if (sequence->peekFirst() == NULL && !sequence->isRunning()) {
            sequence->setParallel();
        } else {
            status = PARSE_ERROR;
            errorMessage = "% only valid on first command of sequence";
            in->closeCommand();
            end = '\n';
            return false;
        }
    }
    parsed = true;
    while (true) {
        if (in->hasNext()) {
            eatWhitespace(in);
        }
        if (in->hasNext()) {
            c = in->read();
            if (c >= 'A' && c <= 'Z') {
                if (map.has(c)) {
                    status = PARSE_ERROR;
                    errorMessage = "Same field appears twice";
                    in->closeCommand();
                    end = '\n';
                    return false;
                } else if (!parseHexField(in, c)) {
                    status = TOO_BIG;
                    errorMessage = "Too many fields";
                    in->closeCommand();
                    end = '\n';
                    return false;
                }
            } else if (c == '+') {
                if (target->getLength() != 0) {
                    status = PARSE_ERROR;
                    errorMessage = "Multiple big fields";
                    in->closeCommand();
                    end = '\n';
                    return false;
                }
                target->setIsString(false);
                uint8_t d = getHex(in->peek());

                while (in->hasNext() && isHex(in->peek())) {
                    d = getHex(in->read());
                    d <<= 4;
                    if (!in->hasNext() || !isHex(in->peek())) {
                        status = PARSE_ERROR;
                        errorMessage = "Big field odd digits";
                        in->closeCommand();
                        end = '\n';
                        return false;
                    }
                    d += getHex(in->read());
                    if (!target->addByteToBigField(d)) {
                        if (target == &big
                                && RCodeParameters::bigBigFieldLength > 0) {
                            usesBigBig = true;
                            target = bigBig;
                            bigBig->copyFrom(&big);
                            bigBig->addByteToBigField(d);
                        } else {
                            status = TOO_BIG;
                            errorMessage = "Big field too long";
                            in->closeCommand();
                            end = '\n';
                            return false;
                        }
                    }
                }
            } else if (c == '"') {
                if (target->getLength() != 0) {
                    status = PARSE_ERROR;
                    errorMessage = "Multiple big fields";
                    in->closeCommand();
                    end = '\n';
                    return false;
                }
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
                            if (target == &big
                                    && RCodeParameters::bigBigFieldLength > 0) {
                                usesBigBig = true;
                                target = bigBig;
                                bigBig->copyFrom(&big);
                                bigBig->addByteToBigField((uint8_t) c);
                            } else {
                                status = TOO_BIG;
                                errorMessage = "Big field too long";
                                in->closeCommand();
                                end = '\n';
                                return false;
                            }
                        }
                    } else {
                        break;
                    }
                }
                if (c != '"') {
                    status = PARSE_ERROR;
                    errorMessage =
                            "Command sequence ended before end of big string field";
                    in->closeCommand();
                    end = '\n';
                    return false;
                }
            } else {
                status = PARSE_ERROR;
                errorMessage = "Unknown field marker";
                in->closeCommand();
                end = '\n';
                return false;
            }
        } else {
            in->closeCommand();
            end = in->read();
            if (getCommand(sequence->getRCode()) == NULL) {
                status = UNKNOWN_CMD;
                errorMessage = "Command not known";
                end = '\n';
                return false;
            }
            return true;
        }
    }

}

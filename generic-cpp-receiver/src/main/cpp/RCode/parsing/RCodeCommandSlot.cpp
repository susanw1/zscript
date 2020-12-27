/*
 * RCodeCommandSlot.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#include "RCodeCommandSlotInterrim.hpp"

void RCodeCommandSlot::failParse(RCodeCommandInStream *in,
        RCodeCommandSequence *sequence, RCodeResponseStatus errorStatus,
        char const *errorMessage) {
    status = errorStatus;
    this->errorMessage = errorMessage;

    sequence->getRCode()->getDebug() << errorMessage << "\n";
    in->close();
    end = '\n';
}
bool RCodeCommandSlot::parseHexField(RCodeCommandInStream *in, char field) {
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
            for (uint8_t i = 0; i < sizeof(fieldUnit) * 2; i++) {
                current <<= 4;
                current |= getHex(in->read());
            }
            lookahead -= sizeof(fieldUnit) * 2;
            if (!map.add(field, current)) {
                return false;
            }
        }
        return true;

    }
}
RCodeCommand* RCodeCommandSlot::getCommand(RCode *rcode) {
    if (cmd == NULL && !slotStatus.hasCheckedCommand) {
        slotStatus.hasCheckedCommand = true;
        cmd = rcode->getCommandFinder()->findCommand(this);
    }
    return cmd;
}
bool RCodeCommandSlot::parseSingleCommand(RCodeCommandInStream *in,
        RCodeCommandSequence *sequence) {
    RCodeBigField *target = &big;
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
                    failParse(in, sequence, PARSE_ERROR,
                            "Same field appears twice");
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
                    d = getHex(in->read());
                    d <<= 4;
                    if (!in->hasNext() || !isHex(in->peek())) {
                        slotStatus.hasCheckedCommand = true;
                        failParse(in, sequence, PARSE_ERROR,
                                "Big field odd digits");
                        return false;
                    }
                    d += getHex(in->read());
                    if (!target->addByteToBigField(d)) {
                        if (target == &big
                                && RCodeParameters::bigBigFieldLength > 0) {
                            slotStatus.usesBigBig = true;
                            target = bigBig;
                            bigBig->copyFrom(&big);
                            bigBig->addByteToBigField(d);
                        } else {
                            slotStatus.hasCheckedCommand = true;
                            failParse(in, sequence, TOO_BIG,
                                    "Big field too long");
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
                            if (target == &big
                                    && RCodeParameters::bigBigFieldLength > 0) {
                                slotStatus.usesBigBig = true;
                                target = bigBig;
                                bigBig->copyFrom(&big);
                                bigBig->addByteToBigField((uint8_t) c);
                            } else {
                                slotStatus.hasCheckedCommand = true;
                                failParse(in, sequence, TOO_BIG,
                                        "Big field too long");
                                return false;
                            }
                        }
                    } else {
                        break;
                    }
                }
                if (c != '"') {
                    slotStatus.hasCheckedCommand = true;
                    failParse(in, sequence, PARSE_ERROR,
                            "Command sequence ended before end of big string field");
                    return false;
                }
            } else {
                slotStatus.hasCheckedCommand = true;
                sequence->getRCode()->getDebug() << "Unknown field marker: "
                        << c << "\n";
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

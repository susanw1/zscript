/*
 * RCodeCommandSlot.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#include "RCodeCommandSlotInterrim.hpp"

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
            for (int i = 0; i < sizeof(fieldUnit) * 2; i++) {
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
bool RCodeCommandSlot::parseSingleCommand(RCodeInStream *in,
        RCodeCommandSequence *sequence) {
    RCodeBigField *target = &big;
    in->openCommand();
    reset();
    RCodeParser::eatWhitespace(in);
    if (!in->hasNext()) {
        status = PARSE_ERROR;
        errorMessage = "No command present";
        sequence->getRCode()->getDebug() << "No command present\n";
        in->closeCommand();
        end = '\n';
        return false;
    }
    char c;
    slotStatus.parsed = true;
    while (true) {
        if (in->hasNext()) {
            RCodeParser::eatWhitespace(in);
        }
        if (in->hasNext()) {
            c = in->read();
            if (c >= 'A' && c <= 'Z') {
                if (map.has(c)) {
                    slotStatus.hasCheckedCommand = true;
                    status = PARSE_ERROR;
                    errorMessage = "Same field appears twice";
                    sequence->getRCode()->getDebug()
                            << "Same field appears twice\n";
                    in->closeCommand();
                    end = '\n';
                    return false;
                } else if (!parseHexField(in, c)) {
                    slotStatus.hasCheckedCommand = true;
                    status = TOO_BIG;
                    errorMessage = "Too many fields";
                    sequence->getRCode()->getDebug() << "Too many fields\n";
                    in->closeCommand();
                    end = '\n';
                    return false;
                }
            } else if (c == '+') {
                if (target->getLength() != 0) {
                    slotStatus.hasCheckedCommand = true;
                    status = PARSE_ERROR;
                    errorMessage = "Multiple big fields";
                    sequence->getRCode()->getDebug() << "Multiple big fields\n";
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
                        slotStatus.hasCheckedCommand = true;
                        status = PARSE_ERROR;
                        errorMessage = "Big field odd digits";
                        sequence->getRCode()->getDebug()
                                << "Big field odd digits\n";
                        in->closeCommand();
                        end = '\n';
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
                            errorMessage = "Big field too long";
                            status = TOO_BIG;
                            sequence->getRCode()->getDebug()
                                    << "Big field too long\n";
                            in->closeCommand();
                            end = '\n';
                            return false;
                        }
                    }
                }
            } else if (c == '"') {
                if (target->getLength() != 0) {
                    slotStatus.hasCheckedCommand = true;
                    status = PARSE_ERROR;
                    errorMessage = "Multiple big fields";
                    sequence->getRCode()->getDebug() << "Multiple big fields\n";
                    in->closeCommand();
                    end = '\n';
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
                                sequence->getRCode()->getDebug()
                                        << "Big field too long\n";
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
                    slotStatus.hasCheckedCommand = true;
                    sequence->getRCode()->getDebug()
                            << "Command sequence ended before end of big string field\n";
                    status = PARSE_ERROR;
                    errorMessage =
                            "Command sequence ended before end of big string field";
                    in->closeCommand();
                    end = '\n';
                    return false;
                }
            } else {
                slotStatus.hasCheckedCommand = true;
                sequence->getRCode()->getDebug() << "Unknown field marker: "
                        << c << "\n";
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

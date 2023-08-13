/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_ZSCRIPTTOKENIZER_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZSCRIPTTOKENIZER_HPP_

#include "../ZscriptIncludes.hpp"
#include "TokenRingBuffer.hpp"

namespace Zscript {

template<class ZP>
class ZscriptTokenizer {
public:
    static const uint8_t ADDRESSING_FIELD_KEY = 0x80;

    static const uint8_t NORMAL_SEQUENCE_END = 0xf0;
    static const uint8_t ERROR_BUFFER_OVERRUN = 0xf1;
    static const uint8_t ERROR_CODE_ODD_BIGFIELD_LENGTH = 0xf2;
    static const uint8_t ERROR_CODE_FIELD_TOO_LONG = 0xf3;
    static const uint8_t ERROR_CODE_STRING_NOT_TERMINATED = 0xf4;
    static const uint8_t ERROR_CODE_STRING_ESCAPING = 0xf5;
    static const uint8_t ERROR_CODE_ILLEGAL_TOKEN = 0xf6;

    static const uint8_t CMD_END_ANDTHEN = 0xe1;
    static const uint8_t CMD_END_ORELSE = 0xe2;
    static const uint8_t CMD_END_OPEN_PAREN = 0xe3;
    static const uint8_t CMD_END_CLOSE_PAREN = 0xe4;

private:
    static const bool DROP_COMMENTS = false;

    TokenRingWriter<ZP> writer;
    const uint8_t maxNumericBytes :4;
    bool skipToNL :1;
    bool bufferOvr :1;
    bool tokenizerError :1;
    bool numeric :1;

    uint8_t escapingCount :2; // 2 bit counter, from 2 to 0
    bool addressing :1;
    bool isText :1;
    bool isNormalString :1;

public:

    ZscriptTokenizer(TokenRingWriter<ZP> writer, uint8_t maxNumericBytes) :
            writer(writer),
                    maxNumericBytes(maxNumericBytes & 0xF), skipToNL(false), bufferOvr(false), tokenizerError(false), numeric(false),
                    addressing(false), isText(false), isNormalString(false), escapingCount(0) {
    }

private:
    void resetFlags() {
        skipToNL = false;
        bufferOvr = false;
        tokenizerError = false;
        numeric = false;
        addressing = false;
        isText = false;
        isNormalString = false;
        escapingCount = 0;
    }

    void acceptText(uint8_t b) {
        if (b == Zchars::EOL_SYMBOL) {
            if (isNormalString) {
                writer.fail(ERROR_CODE_STRING_NOT_TERMINATED);
            } else {
                writer.writeMarker(NORMAL_SEQUENCE_END);
            }
            // we've written some marker, so reset as per the newline:
            resetFlags();
        } else if (escapingCount > 0) {
            uint8_t hex = ZcharsUtils<ZP>::parseHex(b);
            if (hex == ZcharsUtils<ZP>::PARSE_NOT_HEX_0X10) {
                writer.fail(ERROR_CODE_STRING_ESCAPING);
                tokenizerError = true;
            } else {
                writer.continueTokenNibble(hex);
                escapingCount--;
            }
        } else if (isNormalString && b == Zchars::Z_BIGFIELD_QUOTED) {
            writer.endToken();
            isText = false;
        } else if (isNormalString && b == Zchars::Z_STRING_ESCAPE) {
            escapingCount = 2;
        } else {
            writer.continueTokenByte(b);
        }
    }

    void startNewToken(uint8_t b) {
        if (b == Zchars::EOL_SYMBOL) {
            writer.writeMarker(NORMAL_SEQUENCE_END);
            resetFlags();
            return;
        }

        if (addressing && b != Zchars::Z_ADDRESSING_CONTINUE) {
            writer.startToken(ADDRESSING_FIELD_KEY, false);
            writer.continueTokenByte(b);
            addressing = false;
            isText = true;
            escapingCount = 0;
            isNormalString = false;
            return;
        }

        if (ZcharsUtils<ZP>::isSeparator(b)) {
            uint8_t marker = 0;
            switch (b) {
            case Zchars::ANDTHEN_SYMBOL:
                marker = CMD_END_ANDTHEN;
                break;
            case Zchars::ORELSE_SYMBOL:
                marker = CMD_END_ORELSE;
                break;
            case Zchars::OPEN_PAREN_SYMBOL:
                marker = CMD_END_OPEN_PAREN;
                break;
            case Zchars::CLOSE_PAREN_SYMBOL:
                marker = CMD_END_CLOSE_PAREN;
                break;
                // more for other constructs? '(', ')'
            }
            if (marker != 0) {
                writer.writeMarker(marker);
                return;
            }
            //TODO: die
        }

        if (b == Zchars::Z_ADDRESSING) {
            addressing = true;
        }

        if (!ZcharsUtils<ZP>::isAllowableKey(b)) {
            writer.fail(ERROR_CODE_ILLEGAL_TOKEN);
            tokenizerError = true;
            return;
        }

        numeric = !ZcharsUtils<ZP>::isNonNumerical(b);
        isText = false;
        writer.startToken(b, numeric);

        if (b == Zchars::Z_COMMENT) {
            if (DROP_COMMENTS) {
                skipToNL = true;
            } else {
                isText = true;
                isNormalString = false;
                escapingCount = 0;
            }
            return;
        }
        if (b == Zchars::Z_BIGFIELD_QUOTED) {
            isText = true;
            isNormalString = true;
            escapingCount = 0;
            return;
        }
    }

public:

    void dataLost() {
        if (!bufferOvr) {
            writer.fail(ERROR_BUFFER_OVERRUN);
            bufferOvr = true;
        }
    }
    bool checkCapacity() {
        // worst case is... TODO: review this!
        return writer.checkAvailableCapacity(3);
    }
    bool offer(uint8_t b) {
        if (checkCapacity() || writer.getFlags()->isReaderBlocked()) {
            accept(b);
            return true;
        }
        return false;
    }
    void accept(uint8_t b) {
        if ((!isText && ZcharsUtils<ZP>::shouldIgnore(b)) || ZcharsUtils<ZP>::alwaysIgnore(b)) {
            return;
        }

        if (bufferOvr || tokenizerError || skipToNL) {
            if (b == Zchars::EOL_SYMBOL) {
                if (skipToNL) {
                    if (!checkCapacity()) {
                        dataLost();
                        return;
                    }
                    writer.writeMarker(NORMAL_SEQUENCE_END);
                }
                resetFlags();
            }
            return;
        }
        if (!checkCapacity()) {
            dataLost();
            return;
        }

        // TODO: Hysteresis on bufferOvr - review this approach given we're rewinding current token on failure marker
        //        if (bufferOvr) {
        //            if (!writer.checkAvailableCapacity(10)) {
        //                return;
        //            }
        //            bufferOvr = false;
        //        }

        if (writer.isTokenComplete()) {
            startNewToken(b);
            return;
        }

        if (isText) {
            // "text" is broadly interpreted: we're pushing non-numeric bytes into a current token
            acceptText(b);
            return;
        }

        uint8_t hex = ZcharsUtils<ZP>::parseHex(b);
        if (hex != ZcharsUtils<ZP>::PARSE_NOT_HEX_0X10) {
            if (numeric) {
                // Check field length
                int currentLength = writer.getCurrentWriteTokenLength();
                if (currentLength == maxNumericBytes && !writer.isInNibble()) {
                    writer.fail(ERROR_CODE_FIELD_TOO_LONG);
                    tokenizerError = true;
                    return;
                }
                // Skip leading zeros
                if (currentLength == 0 && hex == 0) {
                    return;
                }
            }
            writer.continueTokenNibble(hex);
            return;
        }

        // Check big field odd length
        if (!numeric && writer.getCurrentWriteTokenKey() == Zchars::Z_BIGFIELD_HEX && writer.isInNibble()) {
            writer.fail(ERROR_CODE_ODD_BIGFIELD_LENGTH);
            tokenizerError = true;
            if (b == Zchars::EOL_SYMBOL) {
                // interesting case: the error above could be caused by b==Z_NEWLINE, but we've written an error marker, so just reset and return
                resetFlags();
            }
            return;
        }

        startNewToken(b);
    }

};

}

#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPTTOKENIZER_HPP_ */

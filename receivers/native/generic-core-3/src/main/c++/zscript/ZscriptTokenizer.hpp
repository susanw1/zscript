/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_ZSCRIPTTOKENIZER_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZSCRIPTTOKENIZER_HPP_

#include <iostream>

#include "ZscriptIncludes.hpp"
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

    GenericCore::TokenRingBuffer<ZP> *buffer;
    const uint8_t maxNumericBytes;

    bool skipToNL = false;
    bool bufferOvr = false;
    bool tokenizerError = false;
    bool numeric = false;
    bool addressing = false;

    bool isText = false;
    bool isNormalString = false;
    uint8_t escapingCount = 0; // 2 bit counter, from 2 to 0

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
        if (b == GenericCore::Zchars::EOL_SYMBOL) {
            if (isNormalString) {
                buffer->W_fail(ERROR_CODE_STRING_NOT_TERMINATED);
            } else {
                buffer->W_writeMarker(NORMAL_SEQUENCE_END);
            }
            // we've written some marker, so reset as per the newline:
            resetFlags();
        } else if (escapingCount > 0) {
            uint8_t hex = GenericCore::ZcharsUtils<ZP>::parseHex(b);
            if (hex == GenericCore::ZcharsUtils<ZP>::PARSE_NOT_HEX_0X10) {
                buffer->W_fail(ERROR_CODE_STRING_ESCAPING);
                tokenizerError = true;
            } else {
                buffer->W_continueTokenNibble(hex);
                escapingCount--;
            }
        } else if (isNormalString && b == GenericCore::Zchars::BIGFIELD_QUOTE_MARKER) {
            buffer->W_endToken();
            isText = false;
        } else if (isNormalString && b == GenericCore::Zchars::STRING_ESCAPE_SYMBOL) {
            escapingCount = 2;
        } else {
            buffer->W_continueTokenByte(b);
        }
    }

    void startNewToken(uint8_t b) {
        if (b == GenericCore::Zchars::EOL_SYMBOL) {
            buffer->W_writeMarker(NORMAL_SEQUENCE_END);
            resetFlags();
            return;
        }

        if (addressing && b != GenericCore::Zchars::Z_ADDRESSING_CONTINUE) {
            buffer->W_startToken(ADDRESSING_FIELD_KEY, false);
            buffer->W_continueTokenByte(b);
            addressing = false;
            isText = true;
            escapingCount = 0;
            isNormalString = false;
            return;
        }

        if (GenericCore::ZcharsUtils<ZP>::isSeparator(b)) {
            uint8_t marker = 0;
            switch (b) {
            case GenericCore::Zchars::ANDTHEN_SYMBOL:
                marker = CMD_END_ANDTHEN;
                break;
            case GenericCore::Zchars::ORELSE_SYMBOL:
                marker = CMD_END_ORELSE;
                break;
            case GenericCore::Zchars::OPEN_PAREN_SYMBOL:
                marker = CMD_END_OPEN_PAREN;
                break;
            case GenericCore::Zchars::CLOSE_PAREN_SYMBOL:
                marker = CMD_END_CLOSE_PAREN;
                break;
                // more for other constructs? '(', ')'
            }
            if (marker != 0) {
                buffer->W_writeMarker(marker);
                return;
            }
            //TODO: die
        }

        if (b == GenericCore::Zchars::Z_ADDRESSING) {
            addressing = true;
        }

        if (!GenericCore::ZcharsUtils<ZP>::isAllowableKey(b)) {
            buffer->W_fail(ERROR_CODE_ILLEGAL_TOKEN);
            tokenizerError = true;
            return;
        }

        numeric = !GenericCore::ZcharsUtils<ZP>::isNonNumerical(b);
        isText = false;
        buffer->W_startToken(b, numeric);

        if (b == GenericCore::Zchars::Z_COMMENT) {
            if (DROP_COMMENTS) {
                skipToNL = true;
            } else {
                isText = true;
                isNormalString = false;
                escapingCount = 0;
            }
            return;
        }
        if (b == GenericCore::Zchars::BIGFIELD_QUOTE_MARKER) {
            isText = true;
            isNormalString = true;
            escapingCount = 0;
            return;
        }
    }

public:

    ZscriptTokenizer(GenericCore::TokenRingBuffer<ZP> *buffer, uint8_t maxNumericBytes) :
            buffer(buffer), maxNumericBytes(maxNumericBytes) {
    }
    void dataLost() {
        if (!bufferOvr) {
            buffer->W_fail(ERROR_BUFFER_OVERRUN);
            bufferOvr = true;
        }
    }
    bool checkCapacity() {
        // worst case is... TODO: review this!
        return buffer->W_checkAvailableCapacity(3);
    }
    bool offer(uint8_t b) {
        if (checkCapacity() || buffer->getFlags()->isReaderBlocked()) {
            accept(b);
            return true;
        }
        return false;
    }
    void accept(uint8_t b) {
        if ((!isText && GenericCore::ZcharsUtils<ZP>::shouldIgnore(b)) || GenericCore::ZcharsUtils<ZP>::alwaysIgnore(b)) {
            return;
        }

        if (bufferOvr || tokenizerError || skipToNL) {
            if (b == GenericCore::Zchars::EOL_SYMBOL) {
                if (skipToNL) {
                    if (!checkCapacity()) {
                        dataLost();
                        return;
                    }
                    buffer->W_writeMarker(NORMAL_SEQUENCE_END);
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
        //            if (!buffer->W_checkAvailableCapacity(10)) {
        //                return;
        //            }
        //            bufferOvr = false;
        //        }

        if (buffer->W_isTokenComplete()) {
            startNewToken(b);
            return;
        }

        if (isText) {
            // "text" is broadly interpreted: we're pushing non-numeric bytes into a current token
            acceptText(b);
            return;
        }

        uint8_t hex = GenericCore::ZcharsUtils<ZP>::parseHex(b);
        if (hex != GenericCore::ZcharsUtils<ZP>::PARSE_NOT_HEX_0X10) {
            if (numeric) {
                // Check field length
                int currentLength = buffer->W_getCurrentWriteTokenLength();
                if (currentLength == maxNumericBytes && !buffer->W_isInNibble()) {
                    buffer->W_fail(ERROR_CODE_FIELD_TOO_LONG);
                    tokenizerError = true;
                    return;
                }
                // Skip leading zeros
                if (currentLength == 0 && hex == 0) {
                    return;
                }
            }
            buffer->W_continueTokenNibble(hex);
            return;
        }

        // Check big field odd length
        if (!numeric && buffer->W_getCurrentWriteTokenKey() == GenericCore::Zchars::BIGFIELD_PREFIX_MARKER && buffer->W_isInNibble()) {
            buffer->W_fail(ERROR_CODE_ODD_BIGFIELD_LENGTH);
            tokenizerError = true;
            if (b == GenericCore::Zchars::EOL_SYMBOL) {
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

/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef SRC_MAIN_C___ZSCRIPT_ZCHARS_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZCHARS_HPP_
#include "ZscriptIncludes.hpp"

namespace Zscript {
namespace GenericCore {

enum Zchars {
    CMD_PARAM = 'Z',
    STATUS_RESP_PARAM = 'S',

    ANDTHEN_SYMBOL = '&',
    ORELSE_SYMBOL = '|',
    OPEN_PAREN_SYMBOL = '(',
    CLOSE_PAREN_SYMBOL = ')',
    EOL_SYMBOL = '\n',
    STRING_ESCAPE_SYMBOL = '=',

    BIGFIELD_PREFIX_MARKER = '+',
    BIGFIELD_QUOTE_MARKER = '"',

    ECHO_PREFIX = '_',
    LOCKING_PREFIX = '%',
    COMMENT_PREFIX = '#',
    NOTIFY_PREFIX = '!',
    ADDRESS_PREFIX = '@',
    ADDRESS_SEPARATOR = '.'
};
template<class ZP>
class ZcharsUtils {
public:
    static const int PARSE_NOT_HEX_0X10 = 0x10;

    static uint8_t parseHex(uint8_t b) {
        if (b >= '0' && b <= '9') {
            return b - '0';
        } else if (b >= 'a' && b <= 'f') {
            return b - 'a' + 10;
        } else {
            return PARSE_NOT_HEX_0X10;
        }
    }

    static bool alwaysIgnore(uint8_t b) {
        return b == '\0';
    }

    static bool shouldIgnore(uint8_t b) {
        return b == ' ' || b == '\r' || b == '\t' || b == ',' || b == '\0';
    }

    static bool isSeparator(uint8_t b) {
        return b == EOL_SYMBOL || b == ANDTHEN_SYMBOL || b == ORELSE_SYMBOL || b == OPEN_PAREN_SYMBOL || b == CLOSE_PAREN_SYMBOL;
    }

    static bool isNonNumerical(uint8_t b) {
        return b == LOCKING_PREFIX || b == BIGFIELD_PREFIX_MARKER || b == BIGFIELD_QUOTE_MARKER || b == COMMENT_PREFIX;
    }

    static bool isBigField(uint8_t b) {
        return b == BIGFIELD_PREFIX_MARKER || b == BIGFIELD_QUOTE_MARKER;
    }

    static bool isNumericKey(uint8_t b) {
        return (b >= 'A' && b <= 'Z');
    }

    static bool isCommandKey(uint8_t b) {
        return isNumericKey(b) || isBigField(b);
    }

    static bool isAllowableKey(uint8_t b) {
        // disallow hex, non-printing and top-bit-set keys
        return b >= 0x20 && b < 0x80 && parseHex(b) == PARSE_NOT_HEX_0X10;
    }
};

}
}
#endif /* SRC_MAIN_C___ZSCRIPT_ZCHARS_HPP_ */

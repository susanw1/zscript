/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_ZSCRIPTABSTRACTOUTSTREAM_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZSCRIPTABSTRACTOUTSTREAM_HPP_
#include "ZscriptIncludes.hpp"

namespace Zscript {
template<class ZP>
class AbstractOutStream {
protected:
    static uint8_t toHexChar(uint8_t b) {
        uint8_t lowNibble = b & 0xf;
        return (lowNibble < 10 ? lowNibble + '0' : lowNibble + ('a' - 10));
    }
    static void appendHex(uint8_t b, uint8_t *buffer, uint8_t index) {
        buffer[index] = toHexChar(b >> 4);
        buffer[index + 1] = toHexChar(b & 0xf);
    }
    static uint8_t appendHexTrim(uint8_t b, uint8_t *buffer, uint8_t index) {
        if ((b & 0xF0) != 0) {
            buffer[index] = toHexChar(b >> 4);
            buffer[index + 1] = toHexChar(b & 0xf);
            return 2;
        }
        if (b != 0) {
            buffer[index] = toHexChar(b & 0xf);
            return 1;
        }
        return 0;
    }

    virtual void writeBytes(uint8_t *bytes, uint16_t count, bool hexMode) = 0;

public:
    virtual void open() = 0;
    virtual void close() = 0;
    virtual bool isOpen() = 0;

    void endSequence() {
        uint8_t c = '\n';
        writeBytes(&c, 1, false);
    }
    void writeField(uint8_t field, uint16_t value) {
        uint8_t bytes[5];
        bytes[0] = field;

        uint8_t offset = appendHexTrim((uint8_t) (value >> 8), bytes, 1);
        if (offset > 0) {
            appendHex((uint8_t) (value & 0xFF), bytes, offset + 1);
            offset += 2;
        } else {
            offset += appendHexTrim((uint8_t) (value & 0xFF), bytes, offset + 1);
        }
        writeBytes(bytes, offset + 1, false);
    }
    void writeOrElse() {
        uint8_t c = '|';
        writeBytes(&c, 1, false);
    }

    void writeAndThen() {
        uint8_t c = '&';
        writeBytes(&c, 1, false);
    }

    void writeOpenParen() {
        uint8_t c = '(';
        writeBytes(&c, 1, false);
    }

    void writeCloseParen() {
        uint8_t c = ')';
        writeBytes(&c, 1, false);
    }

};
}
#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPTABSTRACTOUTSTREAM_HPP_ */

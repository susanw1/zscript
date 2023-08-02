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
    static void appendHex(uint8_t b, uint8_t *buffer) {
        buffer[0] = toHexChar(b >> 4);
        buffer[1] = toHexChar(b & 0xf);
    }
    static uint8_t appendHexTrim(uint8_t b, uint8_t *buffer) {
        if ((b & 0xF0) != 0) {
            buffer[0] = toHexChar(b >> 4);
            buffer[1] = toHexChar(b & 0xf);
            return 2;
        }
        if (b != 0) {
            buffer[0] = toHexChar(b & 0xf);
            return 1;
        }
        return 0;
    }

    virtual void writeBytes(uint8_t *bytes, uint16_t count, bool hexMode) = 0;

    void escapeStrChar(uint8_t c) {
        uint8_t bytes[3];
        bytes[0] = Zchars::Z_STRING_ESCAPE;

        appendHex(c, bytes + 1);

        writeBytes(bytes, 3, false);
    }

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

        uint8_t offset = appendHexTrim((uint8_t) (value >> 8), bytes + 1);
        if (offset > 0) {
            appendHex((uint8_t) (value & 0xFF), bytes + offset + 1);
            offset += 2;
        } else {
            offset += appendHexTrim((uint8_t) (value & 0xFF), bytes + offset + 1);
        }
        writeBytes(bytes, offset + 1, false);
    }
    void writeBytes(uint8_t *data, uint16_t length) {
        writeBytes(data, length, false);
    }
    void writeBytes(DataArrayWLeng8 data) {
        writeBytes(data.data, data.length, false);
    }
    void writeBytes(DataArrayWLeng16 data) {
        writeBytes(data.data, data.length, false);
    }
    void writeBytes(DataArrayWLeng32 data) {
        writeBytes(data.data, data.length, false);
    }

    void beginBigString() {
        uint8_t c = '"';
        writeBytes(&c, 1, false);
    }
    void continueBigString(DataArrayWLeng8 data) {
        uint8_t segStart = 0;
        for (uint8_t i = 0; i < data.length; ++i) {
            if (ZcharsUtils<ZP>::needsStringEscape(data.data[i])) {
                if (segStart != i) {
                    writeBytes(data.data + segStart, i - segStart, false);
                }
                segStart = i + 1;
                escapeStrChar(data.data[i]);
            }
        }
    }
    void continueBigString(DataArrayWLeng16 data) {
        uint16_t segStart = 0;
        for (uint16_t i = 0; i < data.length; ++i) {
            if (ZcharsUtils<ZP>::needsStringEscape(data.data[i])) {
                if (segStart != i) {
                    writeBytes(data.data + segStart, i - segStart, false);
                }
                segStart = i + 1;
                escapeStrChar(data.data[i]);
            }
        }
    }
    void continueBigString(uint8_t *data, uint16_t length) {
        uint16_t segStart = 0;
        for (uint16_t i = 0; i < length; ++i) {
            if (ZcharsUtils<ZP>::needsStringEscape(data[i])) {
                if (segStart != i) {
                    writeBytes(data + segStart, i - segStart, false);
                }
                segStart = i + 1;
                escapeStrChar(data[i]);
            }
        }
    }

    void endBigString() {
        uint8_t c = '"';
        writeBytes(&c, 1, false);
    }
    void beginBigHex() {
        uint8_t c = '+';
        writeBytes(&c, 1, false);
    }
    void continueBigHex(DataArrayWLeng8 data) {
        writeBytes(data.data, data.length, true);
    }
    void continueBigHex(DataArrayWLeng16 data) {
        writeBytes(data.data, data.length, true);
    }
    void continueBigHex(uint8_t *data, uint16_t length) {
        writeBytes(data, length, true);
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

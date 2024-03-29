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

    virtual void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) = 0;

    void escapeStrChar(uint8_t c) {
        uint8_t bytes[3];
        bytes[0] = Zchars::Z_STRING_ESCAPE;

        appendHex(c, bytes + 1);

        writeBytes(bytes, 3, false);
    }

public:
    virtual void open(uint8_t sourceInd) = 0;

    virtual void close() = 0;

    virtual bool isOpen() = 0;

    void endSequence() {
        uint8_t c = '\n';
        writeBytes(&c, 1, false);
    }

    void writeField(uint8_t field, uint16_t value) {
        uint8_t bytes[5];
        bytes[0] = field;
        uint8_t offset = 0;

        uint8_t byteToTrim = (uint8_t) (value >> 8);
        if (byteToTrim == 0) {
            byteToTrim = (uint8_t) (value & 0xFF);
        }
        if (byteToTrim != 0) {
            offset = appendHexTrim(byteToTrim, bytes + 1);
            if (value >= 0x100) {
                appendHex((uint8_t) (value & 0xFF), bytes + offset + 1);
                offset += 2;
            }
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
        if (segStart != data.length) {
            writeBytes(data.data + segStart, data.length - segStart, false);
        }
    }

    void continueBigString(DataArrayWLeng16 data) {
        continueBigString(data.data, data.length);
    }

    void continueBigString(const uint8_t *data, uint16_t length) {
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
        if (segStart != length) {
            writeBytes(data + segStart, length - segStart, false);
        }
    }

    void continueBigString(const char *text) {
        uint16_t segStart = 0;
        uint16_t i;
        for (i = 0; text[i] != 0; ++i) {
            if (ZcharsUtils<ZP>::needsStringEscape(text[i])) {
                if (segStart != i) {
                    writeBytes((const uint8_t *) text + segStart, i - segStart, false);
                }
                segStart = i + 1;
                escapeStrChar(text[i]);
            }
        }
        if (segStart != i) {
            writeBytes((const uint8_t *) text + segStart, i - segStart, false);
        }
    }

    void writeQuotedString(const char *text) {
        beginBigString();
        continueBigString(text);
        endBigString();
    }

    void writeQuotedString(DataArrayWLeng8 data) {
        beginBigString();
        continueBigString(data);
        endBigString();
    }

    void writeQuotedString(DataArrayWLeng16 data) {
        beginBigString();
        continueBigString(data);
        endBigString();
    }

    void writeQuotedString(const uint8_t *data, uint16_t length) {
        beginBigString();
        continueBigString(data, length);
        endBigString();
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

    void continueBigHex(const uint8_t *data, uint16_t length) {
        writeBytes(data, length, true);
    }

    void continueBigHex(const char *text) {
        uint16_t length = 0;
        while (text[length] != 0) {
            length++;
        }
        writeBytes((const uint8_t *) text, length, true);
    }

    void writeBigHex(const char *text) {
        beginBigHex();
        continueBigHex(text);
    }

    void writeBigHex(DataArrayWLeng8 data) {
        beginBigHex();
        continueBigHex(data);
    }

    void writeBigHex(DataArrayWLeng16 data) {
        beginBigHex();
        continueBigHex(data);
    }

    void writeBigHex(const uint8_t *data, uint16_t length) {
        beginBigHex();
        continueBigHex(data, length);
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

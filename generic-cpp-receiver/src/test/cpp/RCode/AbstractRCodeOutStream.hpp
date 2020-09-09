/*
 * AbstractRCodeOutStream.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_ABSTRACTRCODEOUTSTREAM_HPP_
#define SRC_TEST_CPP_RCODE_ABSTRACTRCODEOUTSTREAM_HPP_
#include "climits"
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeOutStream.hpp"

class AbstractRCodeOutStream: public RCodeOutStream {
private:
    uint8_t toHexDigit(int i) {
        return i > 9 ? 'a' + i - 10 : '0' + i;
    }
public:
    virtual void writeByte(uint8_t value) = 0;

    virtual RCodeOutStream* markNotification() {
        writeByte('!');
        return this;
    }

    virtual RCodeOutStream* markBroadcast() {
        writeByte('*');
        return this;
    }

    virtual RCodeOutStream* writeStatus(RCodeResponseStatus st) {
        writeByte('S');
        if (st != OK) {
            if (st > 16) {
                writeByte(toHexDigit((st >> 4) & 0x0F));
            }
            writeByte(toHexDigit(st & 0x0F));
        }
        return this;
    }

    virtual RCodeOutStream* writeField(char f, uint8_t v) {
        writeByte(f);
        if (v != 0) {
            if (v > 16) {
                writeByte(toHexDigit(v >> 4));
            }
            writeByte(toHexDigit(v & 0x0F));
        }
        return this;
    }

    virtual RCodeOutStream* continueField(uint8_t v) {
        if (v > 16) {
            writeByte(toHexDigit(v >> 4));
        }
        writeByte(toHexDigit(v & 0x0F));
        return this;
    }

    virtual RCodeOutStream* writeBigHexField(uint8_t const *value,
            uint16_t length) {
        writeByte('+');
        for (int i = 0; i < length; i++) {
            writeByte(toHexDigit(value[i] >> 4));
            writeByte(toHexDigit(value[i] & 0x0F));
        }
        return this;
    }

    virtual RCodeOutStream* writeBigStringField(uint8_t const *value,
            uint16_t length) {
        writeByte('"');
        for (int i = 0; i < length; i++) {
            if (value[i] == '\n') {
                writeByte('\\');
                writeByte('n');
            } else if (value[i] == '\\' || value[i] == '"') {
                writeByte('\\');
                writeByte(value[i]);
            } else {
                writeByte(value[i]);
            }
        }
        writeByte('"');
        return this;
    }

    virtual RCodeOutStream* writeBigStringField(char const *s) {
        writeByte('"');
        if (sizeof(uint8_t) == sizeof(char)) {
            int i = 0;
            while (*(s + i++) != 0)
                ;
            writeBytes((uint8_t const*) s, i);
        } else {
            int i = 0;
            char c;
            while ((c = *(s + i++)) != 0) {
                writeByte((uint8_t) c);
            }
        }
        writeByte('"');
        return this;
    }

    virtual RCodeOutStream* writeCommandSeperator() {
        writeByte(';');
        return this;
    }

    virtual RCodeOutStream* writeCommandSequenceSeperator() {
        writeByte('\n');
        return this;
    }
    virtual ~AbstractRCodeOutStream() {

    }
};

#endif /* SRC_TEST_CPP_RCODE_ABSTRACTRCODEOUTSTREAM_HPP_ */

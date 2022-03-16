/*
 * AbstractZcodeOutStream.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ABSTRACTZCODEOUTSTREAM_HPP_
#define SRC_TEST_CPP_ZCODE_ABSTRACTZCODEOUTSTREAM_HPP_

#include <climits>
#include <cassert>

#include "ZcodeIncludes.hpp"
#include "ZcodeOutStream.hpp"

template<class ZP>
class AbstractZcodeOutStream: public ZcodeOutStream<ZP> {
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename ZP::fieldUnit_t fieldUnit_t;
private:
    uint8_t toHexDigit(int i) {
        return (uint8_t) (i > 9 ? 'a' + i - 10 : '0' + i);
    }
public:
    virtual void writeByte(uint8_t value) = 0;

    virtual ZcodeOutStream<ZP>* markDebug() {
        writeByte(Zchars::DEBUG_PREFIX);
        return this;
    }
    virtual ZcodeOutStream<ZP>* markNotification() {
        writeByte(Zchars::NOTIFY_PREFIX);
        return this;
    }

    virtual ZcodeOutStream<ZP>* markBroadcast() {
        writeByte(Zchars::BROADCAST_PREFIX);
        return this;
    }

    virtual ZcodeOutStream<ZP>* writeStatus(ZcodeResponseStatus st) {
        writeByte(Zchars::STATUS_RESP_PARAM);
        if (st != OK) {
            if (st >= 0x10) {
                writeByte(toHexDigit((st >> 4) & 0x0F));
            }
            writeByte(toHexDigit(st & 0x0F));
        }
        return this;
    }

    virtual ZcodeOutStream<ZP>* writeField(char f, fieldUnit_t v) {
        writeByte(f);
        if (sizeof(fieldUnit_t) == 1 || v <= 255) {
            if (v != 0) {
                if (v >= 0x10) {
                    writeByte(toHexDigit(v >> 4));
                }
                writeByte(toHexDigit(v & 0x0F));
            }
        } else {
            int howManyNibbles = 2;
            for (fieldUnit_t tmp = 0xff; tmp < v; tmp = (fieldUnit_t) ((tmp << 4) + 0x0f)) {
                howManyNibbles++;
            }
            for (; howManyNibbles > 0; --howManyNibbles) {
                writeByte(toHexDigit((v >> (4 * (howManyNibbles - 1))) & 0x0F));
            }
        }
        return this;
    }

    virtual ZcodeOutStream<ZP>* continueField(fieldUnit_t v) {
        if (sizeof(fieldUnit_t) == 1) {
            writeByte(toHexDigit((v >> 4) & 0x0F));
            writeByte(toHexDigit(v & 0x0F));
        } else {
            int i = sizeof(fieldUnit_t) * 2 - 1;
            for (; i >= 0; --i) {
                writeByte(toHexDigit((v >> (4 * i)) & 0x0F));
            }
        }
        return this;
    }

    virtual ZcodeOutStream<ZP>* writeBigHexField(uint8_t const *value, bigFieldAddress_t length) {
        writeByte(BIGFIELD_PREFIX_MARKER);
        for (int i = 0; i < length; i++) {
            writeByte(toHexDigit(value[i] >> 4));
            writeByte(toHexDigit(value[i] & 0x0F));
        }
        return this;
    }

    virtual ZcodeOutStream<ZP>* writeBigStringField(uint8_t const *value, bigFieldAddress_t length) {
        writeByte(BIGFIELD_QUOTE_MARKER);
        for (bigFieldAddress_t i = 0; i < length; ++i) {
            if (value[i] == Zchars::EOL_SYMBOL) {
                writeByte('\\');
                writeByte('n');
            } else if (value[i] == '\\' || value[i] == BIGFIELD_QUOTE_MARKER) {
                writeByte('\\');
                writeByte(value[i]);
            } else {
                writeByte(value[i]);
            }
        }
        writeByte(BIGFIELD_QUOTE_MARKER);
        return this;
    }

    virtual ZcodeOutStream<ZP>* writeBigStringField(char const *nullTerminated) {
        assert(sizeof(uint8_t) == sizeof(char));
        bigFieldAddress_t i;
        for (i = 0; nullTerminated[i] != '\0'; ++i) {
        }
        writeBigStringField((uint8_t const*) nullTerminated, i);

        return this;
    }

    virtual ZcodeOutStream<ZP>* writeCommandSeparator() {
        writeByte(Zchars::ANDTHEN_SYMBOL);
        return this;
    }
    virtual ZcodeOutStream<ZP>* writeCommandSequenceErrorHandler() {
        writeByte(Zchars::ORELSE_SYMBOL);
        return this;
    }

    virtual ZcodeOutStream<ZP>* writeCommandSequenceSeparator() {
        writeByte(Zchars::EOL_SYMBOL);
        return this;
    }
};

#endif /* SRC_TEST_CPP_ZCODE_ABSTRACTZCODEOUTSTREAM_HPP_ */

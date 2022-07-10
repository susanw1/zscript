/*
 * AbstractZcodeOutStream.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEOUTSTREAM_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEOUTSTREAM_HPP_

#include "ZcodeIncludes.hpp"

template<class ZP>
class ZcodeCommandChannel;

enum ZcodeOutStreamOpenType {
    RESPONSE, NOTIFICATION, DEBUG
};
template<class ZP>
class ZcodeOutStream {
private:
    typedef typename ZP::Strings::string_t string_t;
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;

    uint8_t toHexDigit(uint8_t i) {
        return (uint8_t) (i > 9 ? 'a' + i - 10 : '0' + i);
    }
protected:

    uint16_t recentStatus;
    uint8_t recentBreak;
    bool lockVal = false;

public:
    void *mostRecent;

    virtual void writeByte(uint8_t value) = 0;

    virtual void open(ZcodeCommandChannel<ZP> *target, ZcodeOutStreamOpenType type) = 0;

    virtual bool isOpen() = 0;

    virtual void close() = 0;

    void openResponse(ZcodeCommandChannel<ZP> *target) {
        open(target, RESPONSE);
    }

    void openNotification(ZcodeCommandChannel<ZP> *target) {
        open(target, NOTIFICATION);
    }

    void openDebug(ZcodeCommandChannel<ZP> *target) {
        open(target, DEBUG);
    }

    virtual bool lock() {
        if (!lockVal) {
            lockVal = true;
            return true;
        }
        return false;

    }

    virtual void unlock() {
        lockVal = false;
    }

    virtual bool isLocked() {
        return lockVal;
    }

    ZcodeOutStream<ZP>* writeBytes(uint8_t const *value, uint16_t l) {
        for (uint16_t i = 0; i < l; i++) {
            writeByte(value[i]);
        }
        return this;
    }
    ZcodeOutStream<ZP>* markDebug() {
        writeByte(Zchars::DEBUG_PREFIX);
        return this;
    }
    ZcodeOutStream<ZP>* markNotification() {
        writeByte(Zchars::NOTIFY_PREFIX);
        return this;
    }

    ZcodeOutStream<ZP>* markBroadcast() {
        writeByte(Zchars::BROADCAST_PREFIX);
        return this;
    }

    ZcodeOutStream<ZP>* markAddressing() {
        writeByte(Zchars::ADDRESS_PREFIX);
        return this;
    }

    ZcodeOutStream<ZP>* markAddressingContinue() {
        writeByte(Zchars::ADDRESS_SEPARATOR);
        return this;
    }

    ZcodeOutStream<ZP>* writeStatus(uint16_t st) {
        recentStatus = (uint16_t) st;
        writeByte(Zchars::STATUS_RESP_PARAM);
        if (st != OK) {
            if (st >= 0x10) {
                if (st >= 0x100) {
                    if (st >= 0x1000) {
                        writeByte(toHexDigit((uint8_t) (st >> 12) & 0x0F));
                    }
                    writeByte(toHexDigit((uint8_t) (st >> 8) & 0x0F));
                }
                writeByte(toHexDigit((uint8_t) (st >> 4) & 0x0F));
            }
            writeByte(toHexDigit(st & 0x0F));
        }
        return this;
    }

    ZcodeOutStream<ZP>* writeBigHexField(const uint8_t *value, bigFieldAddress_t length) {
        writeByte(BIGFIELD_PREFIX_MARKER);
        for (bigFieldAddress_t i = 0; i < length; i++) {
            writeByte(toHexDigit((uint8_t) (value[i] >> 4)));
            writeByte(toHexDigit(value[i] & 0x0F));
        }
        return this;
    }

    ZcodeOutStream<ZP>* writeBigStringField(const uint8_t *value, bigFieldAddress_t length) {
        writeByte(BIGFIELD_QUOTE_MARKER);
        for (bigFieldAddress_t i = 0; i < length; ++i) {
            if (value[i] == Zchars::EOL_SYMBOL || value[i] == Zchars::STRING_ESCAPE_SYMBOL || value[i] == BIGFIELD_QUOTE_MARKER) {
                writeByte(Zchars::STRING_ESCAPE_SYMBOL);
                continueField8(value[i]);
            } else {
                writeByte(value[i]);
            }
        }
        writeByte(BIGFIELD_QUOTE_MARKER);
        return this;
    }

    ZcodeOutStream<ZP>* writeBigStringField(string_t nullTerminated) {
        writeByte(BIGFIELD_QUOTE_MARKER);
        char c;
        for (bigFieldAddress_t i = 0; (c = ZP::Strings::getChar(nullTerminated, i)) != 0; ++i) {
            if (c == Zchars::EOL_SYMBOL || c == Zchars::STRING_ESCAPE_SYMBOL || c == BIGFIELD_QUOTE_MARKER) {
                writeByte(Zchars::STRING_ESCAPE_SYMBOL);
                continueField8(c);
            } else {
                writeByte(c);
            }
        }
        writeByte(BIGFIELD_QUOTE_MARKER);
        return this;
    }

    ZcodeOutStream<ZP>* writeBigStringField(const char *nullTerminated) {
        bigFieldAddress_t i;
        for (i = 0; nullTerminated[i] != '\0'; ++i) {
        }
        writeBigStringField((const uint8_t*) nullTerminated, i);

        return this;
    }

    ZcodeOutStream<ZP>* writeCommandSeparator() {
        writeByte(Zchars::ANDTHEN_SYMBOL);
        recentBreak = (uint8_t) Zchars::ANDTHEN_SYMBOL;
        return this;
    }
    ZcodeOutStream<ZP>* writeCommandSequenceErrorHandler() {
        writeByte(Zchars::ORELSE_SYMBOL);
        recentBreak = (uint8_t) Zchars::ORELSE_SYMBOL;
        return this;
    }

    ZcodeOutStream<ZP>* writeCommandSequenceSeparator() {
        writeByte(Zchars::EOL_SYMBOL);
        recentBreak = (uint8_t) Zchars::EOL_SYMBOL;
        return this;
    }

    void writeField8(char f, uint8_t v) {
        writeByte(f);
        writeField8(v);
    }
    void writeField8(uint8_t v) {
        if (v != 0) {
            if (v >= 0x10) {
                writeByte(toHexDigit((uint8_t) (v >> 4)));
            }
            writeByte(toHexDigit(v & 0x0F));
        }
    }
    void continueField8(uint8_t v) {
        writeByte(toHexDigit((uint8_t) (v >> 4)));
        writeByte(toHexDigit(v & 0x0F));
    }

    void writeField16(char f, uint16_t value) {
        if (value <= 0xFF) {
            writeField8(f, (uint8_t) value);
        } else {
            writeField8(f, (uint8_t) (value >> 8));
            continueField8((uint8_t) (value & 0xFF));
        }
    }

    void writeField16(uint16_t value) {
        if (value <= 0xFF) {
            writeField8((uint8_t) value);
        } else {
            writeField8((uint8_t) (value >> 8));
            continueField8((uint8_t) (value & 0xFF));
        }
    }

    void continueField16(uint16_t value) {
        continueField8((uint8_t) (value >> 8));
        continueField8((uint8_t) (value & 0xFF));
    }

    void writeField32(char f, uint32_t value) {
        if (value <= 0xFF) {
            writeField8(f, (uint8_t) value);
        } else if (value <= 0xFFFF) {
            writeField8(f, (uint8_t) (value >> 8));
            continueField8((uint8_t) (value & 0xFF));
        } else {
            writeField8(f, (uint8_t) (value >> 24));
            continueField8((uint8_t) ((value >> 16) & 0xFF));
            continueField8((uint8_t) ((value >> 8) & 0xFF));
            continueField8((uint8_t) (value & 0xFF));
        }
    }

    void writeField32(uint32_t value) {
        if (value <= 0xFF) {
            writeField8((uint8_t) value);
        } else if (value <= 0xFFFF) {
            writeField8((uint8_t) (value >> 8));
            continueField8((uint8_t) (value & 0xFF));
        } else {
            writeField8((uint8_t) (value >> 24));
            continueField8((uint8_t) ((value >> 16) & 0xFF));
            continueField8((uint8_t) ((value >> 8) & 0xFF));
            continueField8((uint8_t) (value & 0xFF));
        }
    }
    void continueField32(uint32_t value) {
        continueField8((uint8_t) (value >> 24));
        continueField8((uint8_t) ((value >> 16) & 0xFF));
        continueField8((uint8_t) ((value >> 8) & 0xFF));
        continueField8((uint8_t) (value & 0xFF));
    }

    void writeField64(uint64_t value) {
        if (value <= 0xFF) {
            writeField8((uint8_t) value);
        } else if (value <= 0xFFFFFFFF) {
            writeField32((uint32_t) value);
        } else {
            writeField32((uint32_t) (value >> 32));
            continueField32((uint32_t) (value & 0xFFFFFFFF));
        }
    }

    void writeField64(char f, uint64_t value) {
        if (value <= 0xFF) {
            writeField8(f, (uint8_t) value);
        } else if (value <= 0xFFFFFFFF) {
            writeField32(f, (uint32_t) value);
        } else {
            writeField32(f, (uint32_t) (value >> 32));
            continueField32((uint32_t) (value & 0xFFFFFFFF));
        }
    }

    void continueField64(uint64_t value) {
        continueField32((uint32_t) (value >> 32));
        continueField32((uint32_t) (value & 0xFFFFFFFF));
    }

    void writeFieldL(char f, const uint8_t *value, uint8_t length) {
        if (length == 0) {
            writeField8(f, 0);
            return;
        } else {
            writeField8(f, *value++);
            length--;
        }
        while (length != 0) {
            length--;
            continueField8(*value++);
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEOUTSTREAM_HPP_ */

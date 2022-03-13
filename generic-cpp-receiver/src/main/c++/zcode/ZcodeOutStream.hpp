/*
 * ZcodeOutStream.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEOUTSTREAM_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEOUTSTREAM_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeResponseStatus.hpp"

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeOutStream {
    typedef typename ZP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename ZP::fieldUnit_t fieldUnit_t;
    public:
    bool lockVal;
    void *mostRecent;

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
    virtual ZcodeOutStream<ZP>* markDebug() = 0;

    virtual ZcodeOutStream<ZP>* markNotification() = 0;

    virtual ZcodeOutStream<ZP>* markBroadcast() = 0;

    virtual ZcodeOutStream<ZP>* writeStatus(ZcodeResponseStatus st) = 0;

    virtual ZcodeOutStream<ZP>* writeField(char f, uint8_t v) = 0;

    virtual ZcodeOutStream<ZP>* continueField(uint8_t v) = 0;

    virtual ZcodeOutStream<ZP>* writeBigHexField(const uint8_t *value, bigFieldAddress_t length) = 0;

    virtual ZcodeOutStream<ZP>* writeBigStringField(const uint8_t *value, bigFieldAddress_t length) = 0;

    virtual ZcodeOutStream<ZP>* writeBigStringField(const char *s) = 0;

    virtual ZcodeOutStream<ZP>* writeBytes(const uint8_t *value, bigFieldAddress_t length) = 0;

    virtual ZcodeOutStream<ZP>* writeCommandSeperator() = 0;

    virtual ZcodeOutStream<ZP>* writeCommandSequenceErrorHandler() = 0;

    virtual ZcodeOutStream<ZP>* writeCommandSequenceSeperator() = 0;

    virtual void openResponse(ZcodeCommandChannel<ZP> *target) = 0;

    virtual void openNotification(ZcodeCommandChannel<ZP> *target) = 0;

    virtual void openDebug(ZcodeCommandChannel<ZP> *target) = 0;

    virtual bool isOpen() = 0;

    virtual void close() = 0;

    void writeField(char f, int8_t value) {
        writeField(f, (uint8_t) value);
    }
    void writeField(char f, uint16_t value) {
        if (value <= 0xFF) {
            writeField(f, (uint8_t) value);
        } else {
            writeField(f, (uint8_t) (value >> 8));
            continueField((uint8_t) (value & 0xFF));
        }
    }
    void writeField(char f, int16_t value) {
        writeField(f, (uint16_t) value);
    }
    void writeField(char f, uint32_t value) {
        if (value <= 0xFF) {
            writeField(f, (uint8_t) value);
        } else if (value <= 0xFFFF) {
            writeField(f, (uint8_t) (value >> 8));
            continueField((uint8_t) (value & 0xFF));
        } else {
            writeField(f, (uint8_t) (value >> 24));
            continueField((uint8_t) ((value >> 16) & 0xFF));
            continueField((uint8_t) ((value >> 8) & 0xFF));
            continueField((uint8_t) (value & 0xFF));
        }
    }

    void writeField(char f, int32_t value) {
        writeField(f, (uint32_t) value);
    }

    void writeField(char f, uint64_t value) {
        if (value <= 0xFF) {
            writeField(f, (uint8_t) value);
        } else if (value <= 0xFFFFFFFF) {
            writeField(f, (uint32_t) value);
        } else {
            writeField(f, (uint32_t) (value >> 32));
            continueField((uint32_t) (value & 0xFFFFFFFF));
        }
    }
    void continueField(uint64_t value) {
        continueField((uint32_t) (value >> 32));
        continueField((uint32_t) (value & 0xFFFFFFFF));
    }
    static void writeField(char f, int64_t value) {
        writeField(f, (uint64_t) value);
    }
    static void continueField(int64_t value) {
        continueField((uint64_t) value);
    }

    void writeField(char f, const uint8_t *value, int length) {
        if (length == 0) {
            writeField(f, 0);
            return;
        }
        uint8_t toWrite = 0;
        while (length != 0) {
            length--;
            continueField(*value++);
        }
    }
private:
    void continueField(uint32_t value) {
        continueField((uint8_t) (value >> 24));
        continueField((uint8_t) ((value >> 16) & 0xFF));
        continueField((uint8_t) ((value >> 8) & 0xFF));
        continueField((uint8_t) (value & 0xFF));
    }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEOUTSTREAM_HPP_ */

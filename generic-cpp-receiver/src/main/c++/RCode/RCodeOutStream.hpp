/*
 * RCodeOutStream.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_
#define SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeResponseStatus.hpp"

template<class RP>
class RCodeCommandChannel;

template<class RP>
class RCodeOutStream {
    typedef typename RP::bigFieldAddress_t bigFieldAddress_t;
    typedef typename RP::fieldUnit_t fieldUnit_t;
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
    virtual RCodeOutStream<RP>* markDebug() = 0;

    virtual RCodeOutStream<RP>* markNotification() = 0;

    virtual RCodeOutStream<RP>* markBroadcast() = 0;

    virtual RCodeOutStream<RP>* writeStatus(RCodeResponseStatus st) = 0;

    virtual RCodeOutStream<RP>* writeField(char f, fieldUnit_t v) = 0;

    virtual RCodeOutStream<RP>* continueField(fieldUnit_t v) = 0;

    virtual RCodeOutStream<RP>* writeBigHexField(uint8_t const *value, bigFieldAddress_t length) = 0;

    virtual RCodeOutStream<RP>* writeBigStringField(uint8_t const *value, bigFieldAddress_t length) = 0;

    virtual RCodeOutStream<RP>* writeBigStringField(char const *s) = 0;

    virtual RCodeOutStream<RP>* writeBytes(uint8_t const *value, bigFieldAddress_t length) = 0;

    virtual RCodeOutStream<RP>* writeCommandSeperator() = 0;

    virtual RCodeOutStream<RP>* writeCommandSequenceErrorHandler() = 0;

    virtual RCodeOutStream<RP>* writeCommandSequenceSeperator() = 0;

    virtual void openResponse(RCodeCommandChannel<RP> *target) = 0;

    virtual void openNotification(RCodeCommandChannel<RP> *target) = 0;

    virtual void openDebug(RCodeCommandChannel<RP> *target) = 0;

    virtual bool isOpen() = 0;

    virtual void close() = 0;

    static void writeFieldType(RCodeOutStream<RP> *target, char f, uint8_t value) {
        target->writeField(f, (fieldUnit_t) value);
    }
    static void writeFieldType(RCodeOutStream<RP> *target, char f, int8_t value) {
        writeFieldType(target, f, (uint8_t) value);
    }
    static void writeFieldType(RCodeOutStream<RP> *target, char f, uint16_t value) {
        if (value < 0xFF) {
            writeFieldType(target, f, (uint8_t) value);
        } else if (sizeof(fieldUnit_t) >= 2) {
            target->writeField(f, (fieldUnit_t) value);
        } else {
            target->writeField(f, (fieldUnit_t) (value >> 8));
            target->continueField((fieldUnit_t) value & 0xFF);
        }
    }
    static void writeFieldType(RCodeOutStream<RP> *target, char f, int16_t value) {
        writeFieldType(target, f, (uint16_t) value);
    }
    static void writeFieldType(RCodeOutStream<RP> *target, char f, uint32_t value) {
        if (value < 0xFF) {
            writeFieldType(target, f, (uint8_t) value);
        } else if (value < 0xFFFF) {
            writeFieldType(target, f, (uint16_t) value);
        } else if (sizeof(fieldUnit_t) >= 4) {
            target->writeField(f, (fieldUnit_t) value);
        } else if (sizeof(fieldUnit_t) == 2) {
            target->writeField(f, (fieldUnit_t) value >> 16);
            target->continueField((fieldUnit_t) value & 0xFFFF);
        } else {
            target->writeField(f, (fieldUnit_t) (value >> 24));
            target->continueField((fieldUnit_t) (value >> 16) & 0xFF);
            target->continueField((fieldUnit_t) (value >> 8) & 0xFF);
            target->continueField((fieldUnit_t) value & 0xFF);
        }
    }

    static void writeFieldType(RCodeOutStream<RP> *target, char f, int32_t value) {
        writeFieldType(target, f, (uint32_t) value);
    }

    static void writeFieldType(RCodeOutStream<RP> *target, char f, uint64_t value) {
        if (value < 0xFF) {
            writeFieldType(target, f, (uint8_t) value);
        } else if (value < 0xFFFF) {
            writeFieldType(target, f, (uint16_t) value);
        } else if (value < 0xFFFFFFFF) {
            writeFieldType(target, f, (uint32_t) value);
        } else if (sizeof(fieldUnit_t) >= 8) {
            target->writeField(f, (fieldUnit_t) value);
        } else {
            writeFieldType(target, f, (uint32_t) value & 0xFFFFFFFF);
            continueFieldType(target, value >> 32);
        }
    }
    static void continueFieldType(RCodeOutStream<RP> *target, uint64_t value) {
        if (sizeof(fieldUnit_t) >= 8) {
            target->continueField((fieldUnit_t) value);
        } else {
            continueFieldType(target, (uint32_t) value & 0xFFFFFFFF);
            continueFieldType(target, value >> 32);
        }
    }
    static void writeFieldType(RCodeOutStream<RP> *target, char f, int64_t value) {
        writeFieldType(target, f, (uint64_t) value);
    }
    static void continueFieldType(RCodeOutStream<RP> *target, int64_t value) {
        continueFieldType(target, (uint64_t) value);
    }

    static void writeFieldType(RCodeOutStream<RP> *target, char f, const uint8_t *value, int length) {
        if (length == 0) {
            target->writeField(f, 0);
            return;
        }
        int leftover = length % sizeof(fieldUnit_t);
        fieldUnit_t toWrite = 0;
        if (leftover == 0) {
            leftover = sizeof(fieldUnit_t);
        }
        for (int i = 0; i < leftover; ++i) {
            toWrite <<= 8;
            toWrite += *value++;
            length--;
        }
        target->writeField(f, toWrite);
        while (length != 0) {
            toWrite = 0;
            for (uint8_t i = 0; i < sizeof(fieldUnit_t); ++i) {
                toWrite <<= 8;
                toWrite += *value++;
                length--;
            }
            target->continueField(toWrite);
        }
    }
private:
    static void continueFieldType(RCodeOutStream<RP> *target, uint32_t value) {
        if (sizeof(fieldUnit_t) >= 4) {
            target->continueField((fieldUnit_t) value);
        } else if (sizeof(fieldUnit_t) == 2) {
            target->continueField((fieldUnit_t) value >> 16);
            target->continueField((fieldUnit_t) value & 0xFFFF);
        } else {
            target->continueField((fieldUnit_t) value >> 24);
            target->continueField((fieldUnit_t) (value >> 16) & 0xFF);
            target->continueField((fieldUnit_t) (value >> 8) & 0xFF);
            target->continueField((fieldUnit_t) value & 0xFF);
        }
    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_ */

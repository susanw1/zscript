/*
 * RCodeOutStream.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_
#define SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeResponseStatus.hpp"

class RCodeCommandChannel;

class RCodeOutStream {
public:
    bool lockVal;
    void *mostRecent;

    bool lock() {
        if (!lockVal) {
            lockVal = true;
            return true;
        }
        return false;

    }

    void unlock() {
        lockVal = false;
    }

    bool isLocked() {
        return lockVal;
    }
    virtual RCodeOutStream* markDebug() = 0;

    virtual RCodeOutStream* markNotification() = 0;

    virtual RCodeOutStream* markBroadcast() = 0;

    virtual RCodeOutStream* writeStatus(RCodeResponseStatus st) = 0;

    virtual RCodeOutStream* writeField(char f, fieldUnit v) = 0;

    virtual RCodeOutStream* continueField(fieldUnit v) = 0;

    virtual RCodeOutStream* writeBigHexField(uint8_t const *value,
            bigFieldAddress_t length) = 0;

    virtual RCodeOutStream* writeBigStringField(uint8_t const *value,
            bigFieldAddress_t length) = 0;

    virtual RCodeOutStream* writeBigStringField(char const *s) = 0;

    virtual RCodeOutStream* writeBytes(uint8_t const *value,
            bigFieldAddress_t length) = 0;

    virtual RCodeOutStream* writeCommandSeperator() = 0;

    virtual RCodeOutStream* writeCommandSequenceErrorHandler() = 0;

    virtual RCodeOutStream* writeCommandSequenceSeperator() = 0;

    virtual void openResponse(RCodeCommandChannel *target) = 0;

    virtual void openNotification(RCodeCommandChannel *target) = 0;

    virtual void openDebug(RCodeCommandChannel *target) = 0;

    virtual bool isOpen() = 0;

    virtual void close() = 0;

    virtual ~RCodeOutStream() {

    }
    static void writeFieldType(RCodeOutStream *target, char f, uint8_t value) {
        target->writeField(f, (fieldUnit) value);
    }
    static void writeFieldType(RCodeOutStream *target, char f, int8_t value) {
        writeFieldType(target, f, (uint8_t) value);
    }
    static void writeFieldType(RCodeOutStream *target, char f, uint16_t value) {
        if (value < 0xFF) {
            writeFieldType(target, f, (uint8_t) value);
        } else if (sizeof(fieldUnit) >= 2) {
            target->writeField(f, (fieldUnit) value);
        } else {
            target->writeField(f, (fieldUnit) (value >> 8));
            target->continueField((fieldUnit) value & 0xFF);
        }
    }
    static void writeFieldType(RCodeOutStream *target, char f, int16_t value) {
        writeFieldType(target, f, (uint16_t) value);
    }
    static void writeFieldType(RCodeOutStream *target, char f, uint32_t value) {
        if (value < 0xFF) {
            writeFieldType(target, f, (uint8_t) value);
        } else if (value < 0xFFFF) {
            writeFieldType(target, f, (uint16_t) value);
        } else if (sizeof(fieldUnit) >= 4) {
            target->writeField(f, (fieldUnit) value);
        } else if (sizeof(fieldUnit) == 2) {
            target->writeField(f, (fieldUnit) value >> 16);
            target->continueField((fieldUnit) value & 0xFFFF);
        } else {
            target->writeField(f, (fieldUnit) value >> 24);
            target->continueField((fieldUnit) (value >> 16) & 0xFF);
            target->continueField((fieldUnit) (value >> 8) & 0xFF);
            target->continueField((fieldUnit) value & 0xFF);
        }
    }
    static void writeFieldType(RCodeOutStream *target, char f, int32_t value) {
        writeFieldType(target, f, (uint32_t) value);
    }
    static void writeFieldType(RCodeOutStream *target, char f, uint64_t value) {
        if (value < 0xFF) {
            writeFieldType(target, f, (uint8_t) value);
        } else if (value < 0xFFFF) {
            writeFieldType(target, f, (uint16_t) value);
        } else if (value < 0xFFFFFFFF) {
            writeFieldType(target, f, (uint32_t) value);
        } else if (sizeof(fieldUnit) >= 8) {
            target->writeField(f, (fieldUnit) value);
        } else {
            writeFieldType(target, f, (uint32_t) value & 0xFFFFFFFF);
            continueFieldType(target, value >> 32);
        }
    }
    static void continueFieldType(RCodeOutStream *target, uint64_t value) {
        if (sizeof(fieldUnit) >= 8) {
            target->continueField((fieldUnit) value);
        } else {
            continueFieldType(target, (uint32_t) value & 0xFFFFFFFF);
            continueFieldType(target, value >> 32);
        }
    }
    static void writeFieldType(RCodeOutStream *target, char f, int64_t value) {
        writeFieldType(target, f, (uint64_t) value);
    }
    static void continueFieldType(RCodeOutStream *target, int64_t value) {
        continueFieldType(target, (uint64_t) value);
    }

    static void writeFieldType(RCodeOutStream *target, char f,
            const uint8_t *value, int length) {

        if (length == 0) {
            target->writeField(f, 0);
            return;
        }
        int leftover = length % sizeof(fieldUnit);
        fieldUnit toWrite = 0;
        if (leftover == 0) {
            leftover = sizeof(fieldUnit);
        }
        for (int i = 0; i < leftover; ++i) {
            toWrite <<= 8;
            toWrite += *value++;
            length--;
        }
        target->writeField(f, toWrite);
        while (length != 0) {
            toWrite = 0;
            for (int i = 0; i < sizeof(fieldUnit); ++i) {
                toWrite <<= 8;
                toWrite += *value++;
                length--;
            }
            target->continueField(toWrite);
        }
    }
private:
    static void continueFieldType(RCodeOutStream *target, uint32_t value) {
        if (sizeof(fieldUnit) >= 4) {
            target->continueField((fieldUnit) value);
        } else if (sizeof(fieldUnit) == 2) {
            target->continueField((fieldUnit) value >> 16);
            target->continueField((fieldUnit) value & 0xFFFF);
        } else {
            target->continueField((fieldUnit) value >> 24);
            target->continueField((fieldUnit) (value >> 16) & 0xFF);
            target->continueField((fieldUnit) (value >> 8) & 0xFF);
            target->continueField((fieldUnit) value & 0xFF);
        }
    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODEOUTSTREAM_HPP_ */

/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_SERIAL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_SERIAL_HPP_

#include <serial-ll/SerialLLInterfaceInclude.hpp>

#include <llIncludes.hpp>

enum SerialEvent {
    SerialNoError = 0,
    SerialNoMoreData = 1,
    SerialParityError = 2,
    SerialNoiseError = 3,
    SerialBreakDetect = 4,
    SerialOverflowError = 5
};

//always does 8 bit bytes, no parity - just for simplicity

struct SerialReadResult {
    SerialEvent event;
    uint16_t length;
};
struct SerialSingleResult {
    SerialEvent event;
    uint8_t value;
};
class Serial {

protected:
    SerialIdentifier id;

public:
    SerialIdentifier getId() {
        return id;
    }
    virtual uint32_t getMaxBaud() = 0;
    virtual uint32_t getMinBaud() = 0;

    virtual void init(void (*volatile bufferOverflowCallback)(SerialIdentifier), uint32_t baud_rate, bool singleNdoubleStop) = 0;
    //the buffer overflow handler can read/skip in the callback to clear space - if it doesn't clear enough space we abort the write

    virtual void setTargetValue(void (*volatile targetValueCallback)(SerialIdentifier), SerialSingleResult targetValue) = 0;
    virtual void clearTargetValue() = 0;

    virtual bool sendBreak() = 0;

    virtual bool clearWriteBuffer() = 0;

    virtual void transmitWriteBuffer() = 0;

    virtual bool write(uint8_t datum) = 0;

    virtual bool write(const uint8_t *buffer, uint16_t length) = 0;
    virtual bool canWrite(uint16_t length) = 0;

    virtual SerialReadResult getEvent(uint16_t length) = 0;

    virtual uint16_t skipBadComs(uint16_t minLength);
    virtual uint16_t skipTo(SerialSingleResult target);

    virtual uint16_t skip(uint16_t length) = 0;
    virtual uint16_t available() = 0;
    virtual SerialReadResult read(uint8_t *buffer, uint16_t length) = 0;
    virtual SerialSingleResult read() = 0; //-1 if no data

    virtual SerialSingleResult peek() = 0; //-1 if no data

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_SERIAL_HPP_ */

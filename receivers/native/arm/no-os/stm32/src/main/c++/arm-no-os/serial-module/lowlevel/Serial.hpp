/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_SERIAL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_SERIAL_HPP_

#include <arm-no-os/llIncludes.hpp>

enum SerialError {
    SerialNoError = 0,
    SerialParityError = 1,
    SerialNoiseError = 2,
    SerialFramingError = 3,
    SerialOverflowError = 32
};

//always does 8 bit bytes, no parity - just for simplicity
class Serial {
//    virtual void init(void (*volatile bufferOverflowCallback)(SerialIdentifier), uint32_t baud_rate, bool singleNdoubleStop) = 0;
//    //the buffer overflow handler can read/skip in the callback to clear space - if it doesn't clear enough space we abort the write
//
//    virtual void setTargetValue(void (*volatile targetValueCallback)(SerialIdentifier), uint8_t targetValue) = 0;
//    virtual void clearTargetValue() = 0;
//    virtual SerialIdentifier getId() = 0;
//    virtual bool write(uint8_t datum) = 0;

//    virtual uint16_t getWriteLength() = 0;
//
//    virtual bool attemptWrite(uint8_t datum) = 0;
//
//    virtual bool canAttemptWrite(uint16_t length) = 0;
//
//    virtual void endWrite(bool complete) = 0;

//    virtual void transmitWriteBuffer() = 0;
//
//    virtual SerialError getError(uint16_t length) = 0;
//
//    virtual uint16_t skip(uint16_t length) = 0;
//    virtual uint16_t available() = 0;
//    virtual int16_t read() = 0; //-1 if no data
//
//    virtual uint16_t availablePeek() = 0;
//    virtual int16_t peek() = 0; //-1 if no data
//
//    virtual void endPeek(bool skipTo) = 0;
//    virtual void resetPeek() = 0;
//    virtual void skipToPeek() = 0;
//
//    virtual int32_t getDistance(uint8_t value) = 0;  //returns the number of bytes until the specified value appears, including the value

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

    virtual void setTargetValue(void (*volatile targetValueCallback)(SerialIdentifier), uint8_t targetValue) = 0;
    virtual void clearTargetValue() = 0;
    virtual bool write(uint8_t datum) = 0;

    virtual bool write(const uint8_t *buffer, uint16_t length) = 0;
    virtual bool canWrite(uint16_t length) = 0;

    virtual bool attemptWrite(const uint8_t *buffer, uint16_t length) = 0; //starts writing without allowing read yet - used to discover if a message will fit
    virtual bool attemptWrite(uint8_t datum) = 0;

    virtual bool canAttemptWrite(uint16_t length) = 0;

    virtual void cancelWrite() = 0; //undoes an attempted write
    virtual void completeWrite() = 0; //allows reading of any attempted write
    virtual void transmitWriteBuffer() = 0;

    virtual SerialError getError(uint16_t length) = 0;

    virtual uint16_t skip(uint16_t length) = 0;
    virtual uint16_t available() = 0;
    virtual uint16_t read(uint8_t *buffer, uint16_t length) = 0;
    virtual int16_t read() = 0; //-1 if no data

    virtual uint16_t availablePeek() = 0;
    virtual uint16_t peek(uint8_t *buffer, uint16_t length) = 0;
    virtual int16_t peek() = 0; //-1 if no data

    virtual void resetPeek() = 0;
    virtual void skipToPeek() = 0;

    virtual int32_t getDistance(uint8_t value) = 0;  //returns the number of bytes until the specified value appears, including the value

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_SERIAL_MODULE_LOWLEVEL_SERIAL_HPP_ */

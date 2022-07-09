/*
 * ZcodeParameters.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_

#include "ZcodeIncludes.hpp"

#define ZCODE_STRING_SURROUND(str) str

#define ZCODE_SUPPORT_SCRIPT_SPACE
#define ZCODE_SUPPORT_INTERRUPT_VECTOR
#define ZCODE_SUPPORT_ADDRESSING
#define ZCODE_GENERATE_NOTIFICATIONS
#define ZCODE_SUPPORT_DEBUG

class ZcodeParameters {
public:
    typedef uint8_t fieldUnit_t;

    typedef uint16_t scriptSpaceAddress_t;
    typedef uint16_t scriptSpaceOutLength_t;
    typedef uint16_t bigFieldAddress_t;
    typedef uint8_t fieldMapSize_t;
    typedef uint8_t commandNum_t;
    typedef uint16_t debugOutputBufferLength_t;
    typedef uint8_t lockNumber_t;

    static const int fieldNum = 20;

    static const bool findInterruptSourceAddress = true;
    static const int interruptStoreNum = 5;
    static const int interruptVectorWorkingNum = 5;
    static const int interruptVectorNum = 5;
    static const uint16_t scriptLength = 1024;
    static const uint16_t scriptBigSize = 16;
    static const uint8_t scriptChannelCount = 1;

    static const uint16_t parseIterationMax = 128;

    static const int scriptOutBufferSize = 32;
    static const int lockNum = 8;
    static const int debugBufferLength = 256;
};

//#define USE_I2C_STATIC_READ_BUFFERS

class ZcodePeripheralParameters {
public:
    static const int i2cReadMaximum = 1024; //if USE_I2C_STATIC_READ_BUFFERS defined, 4 arrays of this length are created - very memory expensive
    static const int i2cBusesPerPhyBus = 2;

    static const int pinNumber = 3;
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_ */
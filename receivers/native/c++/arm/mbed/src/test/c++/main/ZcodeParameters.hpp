/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_CPP_MAIN_ZCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_MAIN_ZCODEPARAMETERS_HPP_

#include "ZcodeIncludes.hpp"

#include <mbed.h>

#define DEBUG
#define NOTIFICATIONS

typedef uint8_t fieldUnit;

typedef uint16_t bigFieldAddress_t;
typedef uint8_t fieldMapSize_t;
typedef uint16_t debugOutputBufferSize_t;
typedef uint8_t lockNumber_t;

class ZcodeParameters {
public:
    static const int bigFieldLength = 32;
    static const int maxParams = 20;
    static const bool cacheFieldResults = true;
    static const bool isUsingInterruptVector = true;
    static const bool findInterruptSourceAddress = true;
    static const int commandNum = 16;
    static const int commandCapabilitiesReturnArrayMaxSize = 32;
    static const int interruptNotificationQueueLength = 5;
    static const int scriptedNotificationQueueLength = 5;
    static const int maxInterruptScripts = 5;
    static const uint16_t executionLength = 128;
    static const int executionOutBufferSize = 32;
    static const int executionInNum = 1;
    static const int maxLocks = 20;
    static const int debugBufferSize = 256;
};
class ZcodeLockValues {
public:
    static const uint8_t i2cPhyBus0Lock = 5;
    static const uint8_t i2cPhyBus1Lock = 6;
    static const uint8_t i2cPhyBus2Lock = 7;
    static const uint8_t i2cPhyBus3Lock = 8;
    static const uint8_t executionSpaceLock = 32;
};

//#define USE_I2C_STATIC_READ_BUFFERS

class ZcodePeripheralParameters {
public:
    static const int i2cReadMaximum = 1024; //if USE_I2C_STATIC_READ_BUFFERS defined, 4 arrays of this length are created - very memory expensive
    static const int i2cBusesPerPhyBus = 2;

};

#endif /* SRC_TEST_CPP_MAIN_ZCODEPARAMETERS_HPP_ */

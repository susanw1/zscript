/*
 * ZcodeParameters.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_

#include "ZcodeIncludes.hpp"

class ZcodeParameters {
    public:
        typedef uint8_t fieldUnit_t;

        typedef uint16_t executionSpaceAddress_t;
        typedef uint16_t executionSpaceOutLength_t;
        typedef uint16_t bigFieldAddress_t;
        typedef uint8_t fieldMapSize_t;
        typedef uint8_t commandNum_t;
        typedef uint16_t debugOutputBufferLength_t;
        typedef uint8_t lockNumber_t;

        static const int bigFieldLength = 32;
        static const int hugeFieldLength = 1024;
        static const int fieldNum = 20;
        static const bool cacheFieldResults = true;
        static const bool isUsingInterruptVector = true;
        static const bool findInterruptSourceAddress = true;
        static const int commandNum = 32;
        static const int commandCapabilitiesReturnArrayMaxSize = 32;
        static const int interruptStoreNum = 5;
        static const int interruptVectorWorkingNum = 5;
        static const int interruptVectorNum = 5;
        static const uint16_t executionLength = 128;
        static const int executionOutBufferSize = 32;
        static const int executionInNum = 1;
        static const int executionOutNum = 1;
        static const int maxParallelRunning = 1;
        static const int slotNum = 1;
        static const int lockNum = 20;
        static const int highestBasicLockNum = 15;
        static const int lowestRwLockNum = 32;
        static const int highestRwLockNum = 47;
        static const bool hasMultiByteCommands = false;
        static const int debugBufferLength = 256;
        static const uint16_t persistentMemorySize = 0;
        static const bool persistentGUID = true;
        static const bool persistentMAC = true;

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

        static const int pinNumber = 3;
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_ */

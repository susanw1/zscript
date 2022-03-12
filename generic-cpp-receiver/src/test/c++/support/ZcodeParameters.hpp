/*
 * ZcodeParameters.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_

#include "ZcodeIncludes.hpp"

class TestParams {
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
    static const int bigBigFieldLength = 1024;
    static const int fieldNum = 20;
    static const bool cacheFieldResults = true;
    static const bool isUsingInterruptVector = true;
    static const bool findInterruptSourceAddress = true;
    static const int commandNum = 16;
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
    static const int highestBasicLockNum = 10;
    static const int lowestRwLockNum = 20;
    static const int highestRwLockNum = 25;
    static const bool hasMultiByteCommands = false;
    static const int debugBufferLength = 256;
    static const uint16_t persistentMemorySize = 256;
    static const bool persistentGUID = true;
    static const bool persistentMAC = true;

    static const uint8_t executionSpaceLock = 20;
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_ */

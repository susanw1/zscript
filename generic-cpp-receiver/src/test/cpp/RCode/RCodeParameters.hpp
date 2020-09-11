/*
 * RCodeParameters.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_RCODE_RCODEPARAMETERS_HPP_
#include "RCodeIncludes.hpp"

typedef uint8_t fieldUnit;

class RCodeParameters {
public:
    static const int bigFieldLength = 32;
    static const int bigBigFieldLength = 0;
    static const int fieldNum = 20;
    static const int fieldSize = 0;
    static const bool cacheFieldResults = true;
    static const bool isUsingInterruptVector = true;
    static const bool findInterruptSourceAddress = true;
    static const int commandNum = 8;
    static const int commandCapabilitiesReturnArrayMaxSize = 32;
    static const int interruptStoreNum = 5;
    static const int interruptVectorWorkingNum = 5;
    static const int interruptVectorNum = 5;
    static const int executionLength = 64;
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
    static const int debugBufferLength = 25;
    static const int uipChannelNum = 3;
};

#endif /* SRC_TEST_CPP_RCODE_RCODEPARAMETERS_HPP_ */

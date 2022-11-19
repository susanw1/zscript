/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_CPP_SUPPORT_ZCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_SUPPORT_ZCODEPARAMETERS_HPP_

// This module doesn't really have a "user" or "platform" - it's just test env
#define ZCODE_IDENTIFY_USER_FIRMWARE_STRING "My Test System!"
#define ZCODE_IDENTIFY_USER_FIRMWARE_VERSION 0x0001

#define ZCODE_IDENTIFY_USER_HARDWARE_STRING "Build Host"
#define ZCODE_IDENTIFY_USER_HARDWARE_VERSION 0x0000

#define ZCODE_IDENTIFY_PLATFORM_FIRMWARE_STRING "Minimal Platform for Core Tests"
#define ZCODE_IDENTIFY_PLATFORM_FIRMWARE_VERSION 0x0000

#define ZCODE_IDENTIFY_PLATFORM_HARDWARE_STRING "PC"
#define ZCODE_IDENTIFY_PLATFORM_HARDWARE_VERSION 0x0000

#include <zcode/ZcodeIncludes.hpp>
#include <zcode/ZcodeStrings.hpp>

#define ZCODE_SUPPORT_SCRIPT_SPACE
#define ZCODE_SUPPORT_INTERRUPT_VECTOR
#define ZCODE_SUPPORT_ADDRESSING
#define ZCODE_GENERATE_NOTIFICATIONS
#define ZCODE_SUPPORT_DEBUG

#ifdef ZCODE_SUPPORT_ADDRESSING
#ifdef ZCODE_SUPPORT_DEBUG
#define DEBUG_ADDRESSING
#endif
#endif

template<class ZP>
class ZcodeModuleAddressRouter;

class TestParams {
public:
    typedef ZcodeStrings<TestParams> Strings;
    typedef ZcodeModuleAddressRouter<TestParams> AddressRouter;

    typedef uint16_t scriptSpaceAddress_t;
    typedef uint16_t scriptSpaceOutLength_t;
    typedef uint16_t bigFieldAddress_t;
    typedef uint8_t fieldMapSize_t;
    typedef uint8_t commandNum_t;
    typedef uint16_t debugOutputBufferLength_t;
    typedef uint8_t lockNumber_t;

    static uint16_t numberGenerator() {
        return 1234;
    }

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
    static const uint16_t scriptOutReadBufferSize = 16;
    static const uint16_t interruptVectorOutReadBufferSize = 32;
    static const int lockNum = 8;
    static const int debugBufferLength = 256;

    static const uint16_t mappingAddressCount = 32;
};

#endif /* SRC_TEST_CPP_SUPPORT_ZCODEPARAMETERS_HPP_ */

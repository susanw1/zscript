/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_CPP_MAIN_ZCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_MAIN_ZCODEPARAMETERS_HPP_

#define ZCODE_IDENTIFY_USER_FIRMWARE_STRING  "Zcode Test Platform for STM32G484"
#define ZCODE_IDENTIFY_USER_FIRMWARE_VERSION 0x0001
#define ZCODE_IDENTIFY_USER_HARDWARE_STRING "Alpha Prototype"
#define ZCODE_IDENTIFY_USER_HARDWARE_VERSION 0x0001

#include "GeneralLLSetup.hpp"

#include <zcode/ZcodeIncludes.hpp>
#include <zcode/ZcodeStrings.hpp>
#include "GeneralLLSetup.hpp"
//#include <arm-no-os/system/ZcodeSystemModule.hpp>

#define ZCODE_SUPPORT_SCRIPT_SPACE
#define ZCODE_SUPPORT_INTERRUPT_VECTOR
#define ZCODE_SUPPORT_ADDRESSING
#define ZCODE_GENERATE_NOTIFICATIONS
#define ZCODE_SUPPORT_DEBUG

#define I2C_ADDRESSING
#define SERIAL_ADDRESSING
#define DEBUG_ADDRESSING

template<class ZP>
class ZcodeModuleAddressRouter;

class ZcodeParameters {
public:
    typedef uint8_t fieldUnit_t;

    typedef uint16_t scriptSpaceAddress_t;
    typedef uint16_t bigFieldAddress_t;
    typedef uint8_t fieldMapSize_t;
    typedef uint16_t debugOutputBufferSize_t;
    typedef uint8_t lockNumber_t;

    typedef ZcodeStrings<ZcodeParameters> Strings;
    typedef ZcodeModuleAddressRouter<ZcodeParameters> AddressRouter;

    static uint16_t numberGenerator() {
        return (uint16_t) ZcodeSystemModule<ZcodeParameters>::milliClock::getTimeMillis();
    }

    typedef LowLevelConfiguration LL;

    static const uint16_t ethernetUdpChannelBigFieldSize = 128;
    static const uint16_t serialChannelBigFieldSize = 128;
    static const uint16_t ethernetUdpChannelReadBufferSize = 64;
    static const uint16_t serialChannelReadBufferSize = 64;

    static const uint8_t maxParams = 20;

    static const bool findInterruptSourceAddress = true;
    static const uint8_t interruptNotificationQueueLength = 5;
    static const uint8_t scriptedNotificationQueueLength = 5;
    static const uint8_t maxInterruptScripts = 5;
    static const uint16_t maxScriptSize = 1024;
    static const uint16_t scriptBigFieldSize = 16;
    static const uint8_t scriptChannelCount = 1;

    static const uint16_t maxParseIterations = 128;

    static const uint8_t scriptOutBufferSize = 32;
    static const uint16_t scriptOutReadBufferSize = 16;
    static const uint16_t interruptVectorOutReadBufferSize = 32;
    static const uint8_t maxLocks = 8;
    static const uint16_t debugBufferSize = 256;

    static const uint8_t i2cAddressingResponseChunkSize = 8; //Really has to be 8...
};

#endif /* SRC_TEST_CPP_MAIN_ZCODEPARAMETERS_HPP_ */

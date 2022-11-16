/*
 * ZcodeParameters.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_

#define ZCODE_IDENTIFY_USER_FIRMWARE_STRING  "Test-minimal STM32F030"
#define ZCODE_IDENTIFY_USER_FIRMWARE_VERSION 0x0001
#define ZCODE_IDENTIFY_HARDWARE_PLATFORM_STRING "ARM STM32F030;Beta Prototype"
#define ZCODE_IDENTIFY_HARDWARE_PLATFORM_VERSION 0x0001

#include <arm-no-os/stm32f030x8/ArmPlatformFirmwareVersion.hpp>

#include <zcode/ZcodeIncludes.hpp>
#include <zcode/ZcodeStrings.hpp>
#include "GeneralLLSetup.hpp"
#include <arm-no-os/system/ZcodeSystemModule.hpp>

//#define ZCODE_SUPPORT_SCRIPT_SPACE
//#define ZCODE_SUPPORT_INTERRUPT_VECTOR
//#define ZCODE_SUPPORT_ADDRESSING
//#define ZCODE_GENERATE_NOTIFICATIONS
//#define ZCODE_SUPPORT_DEBUG

//#define I2C_ADDRESSING
//#define SERIAL_ADDRESSING
//#define DEBUG_ADDRESSING

template<class ZP>
class ZcodeModuleAddressRouter;

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

    static const uint8_t fieldNum = 20;

    static const bool findInterruptSourceAddress = true;
    static const uint8_t interruptStoreNum = 5;
    static const uint8_t interruptVectorWorkingNum = 5;
    static const uint8_t interruptVectorNum = 5;
    static const uint16_t scriptLength = 1024;
    static const uint16_t scriptBigSize = 16;
    static const uint8_t scriptChannelCount = 1;

    static const uint16_t parseIterationMax = 128;

    static const uint8_t scriptOutBufferSize = 32;
    static const uint16_t scriptOutReadBufferSize = 16;
    static const uint16_t interruptVectorOutReadBufferSize = 32;
    static const uint8_t lockNum = 8;
    static const uint16_t debugBufferLength = 256;

    static const uint8_t i2cAddressingResponseChunkSize = 8; //Really has to be 8...
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_ */
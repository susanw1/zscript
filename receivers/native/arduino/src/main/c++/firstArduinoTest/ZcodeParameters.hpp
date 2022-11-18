/*
 * ZcodeParameters.hpp
 *
 *  Created on: 7 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_

#define ZCODE_IDENTIFY_USER_FIRMWARE_STRING         "Zcode Arduino Testing"
#define ZCODE_IDENTIFY_USER_FIRMWARE_VERSION        0x0001

#define ZCODE_IDENTIFY_USER_HARDWARE_STRING     "Arduino Nano"
#define ZCODE_IDENTIFY_USER_HARDWARE_VERSION    0x0000

#include <ArduinoPlatformFirmwareVersion.hpp>

#include <ZcodeIncludes.hpp>
#include <ZcodeStrings.hpp>

// Please note that ZcodeFullInclude will set any #defines necessary to other set #defines - so ZCODE_USE_DEBUG_ADDRESSING_SYSTEM enables ZCODE_SUPPORT_DEBUG

//#define ZCODE_SUPPORT_SCRIPT_SPACE
//#define ZCODE_SUPPORT_INTERRUPT_VECTOR

#define ZCODE_GENERATE_NOTIFICATIONS
#define ZCODE_SUPPORT_DEBUG

#define ZCODE_HAVE_PIN_MODULE
//#define ZCODE_HAVE_I2C_MODULE

#define ZCODE_HAVE_SERIAL_CHANNEL
#define ZCODE_HAVE_I2C_CHANNEL
#define ZCODE_USE_DEBUG_ADDRESSING_SYSTEM

//This is only needed if an addressing system other than ZcodeModuleAddressRouter or ZcodeMappingAddressRouter is used. Otherwise use the below #define
//#define ZCODE_SUPPORT_ADDRESSING

//Please note this needs to line up with the typedef of ZcodeParams::AddressRouter below
#define ZCODE_USE_MODULE_ADDRESSING
//#define ZCODE_USE_MAPPING_ADDRESSING

#define I2C_ENABLE_GENERAL_CALL() TWAR |= 1
#define I2C_SET_ADDRESS(addr) TWAR = addr<<1

template<class ZP>
class ZcodeModuleAddressRouter;
template<class ZP>
class ZcodeMappingAddressRouter;

class ZcodeParams {
public:
    typedef ZcodeStrings<ZcodeParams> Strings;
    typedef ZcodeModuleAddressRouter<ZcodeParams> AddressRouter;

    static uint16_t numberGenerator() {
        return (uint16_t) millis();
    }

    typedef uint16_t scriptSpaceAddress_t;
    typedef uint16_t scriptSpaceOutLength_t;
    typedef uint16_t bigFieldAddress_t;
    typedef uint8_t fieldMapSize_t;
    typedef uint8_t commandNum_t;
    typedef uint16_t debugOutputBufferLength_t;
    typedef uint8_t lockNumber_t;

    static const int fieldNum = 10;

    static const bool findInterruptSourceAddress = true;
    static const int interruptStoreNum = 5;
    static const int interruptVectorWorkingNum = 5;
    static const int interruptVectorNum = 5;
    static const uint16_t scriptLength = 128;
    static const uint16_t scriptBigSize = 16;
    static const uint8_t scriptChannelCount = 1;

    static const uint16_t parseIterationMax = 128;

    static const int scriptOutBufferSize = 32;
    static const uint16_t scriptOutReadBufferSize = 16;
    static const uint16_t interruptVectorOutReadBufferSize = 32;
    static const int lockNum = 8;
    static const int debugBufferLength = 32;

    static const uint16_t serialBigSize = 32;
    static const uint16_t serialChannelReadBufferSize = 8;

    static const uint16_t i2cBigSize = 32;
    static const uint16_t i2cChannelReadBufferSize = 8;
    static const uint16_t i2cChannelOutputBufferSize = 32;
    static const uint16_t i2cAlertPin = 4;

    static const uint16_t mappingAddressCount = 32;

    static const uint8_t pinCount = 21;
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEPARAMETERS_HPP_ */

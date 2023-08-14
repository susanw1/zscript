/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_CONVERTER_FILES_ZSCRIPTDEFAULTPARAMETERS_HPP_
#define SRC_MAIN_CPP_ARDUINO_CONVERTER_FILES_ZSCRIPTDEFAULTPARAMETERS_HPP_

#define ZSCRIPT_IDENTIFY_USER_FIRMWARE_STRING         "Zscript Default Platform for Arduino"
#define ZSCRIPT_IDENTIFY_USER_FIRMWARE_VERSION        0x0001

#define ZSCRIPT_IDENTIFY_USER_HARDWARE_STRING     "Arduino"
#define ZSCRIPT_IDENTIFY_USER_HARDWARE_VERSION    0x0000

#include <ArduinoPlatformFirmwareVersion.hpp>
#include <ZscriptIncludes.hpp>
#include "ZscriptStrings.hpp"

// Please note that ZscriptFullInclude will set any #defines necessary to other set #defines - so eg ZSCRIPT_USE_DEBUG_ADDRESSING_SYSTEM enables ZSCRIPT_SUPPORT_DEBUG

//#define ZSCRIPT_SUPPORT_SCRIPT_SPACE
//#define ZSCRIPT_SUPPORT_INTERRUPT_VECTOR

#define ZSCRIPT_GENERATE_NOTIFICATIONS
//#define ZSCRIPT_SUPPORT_DEBUG

//#define ZSCRIPT_HAVE_I2C_MODULE
#define ZSCRIPT_HAVE_PIN_MODULE

#define ZSCRIPT_HAVE_SERIAL_CHANNEL
//#define ZSCRIPT_HAVE_I2C_CHANNEL
//#define ZSCRIPT_USE_DEBUG_ADDRESSING_SYSTEM

//This is only needed if an addressing system other than ZscriptModuleAddressRouter or ZscriptMappingAddressRouter is used. Otherwise use the below #define
//#define ZSCRIPT_SUPPORT_ADDRESSING

//Please note this needs to line up with the typedef of ZscriptParams::AddressRouter below
#define ZSCRIPT_USE_MODULE_ADDRESSING
//#define ZSCRIPT_USE_MAPPING_ADDRESSING

#define I2C_ENABLE_GENERAL_CALL()
#define I2C_SET_ADDRESS(addr) Wire.end();Wire.begin(addr);

template<class ZP>
class ZscriptModuleAddressRouter;

template<class ZP>
class ZscriptMappingAddressRouter;

class ZscriptParams {
public:
    typedef ZscriptStrings<ZscriptParams> Strings;
    typedef ZscriptModuleAddressRouter<ZscriptParams> AddressRouter;

    static uint16_t numberGenerator() {
        return (uint16_t) millis();
    }

    typedef uint16_t scriptSpaceAddress_t;
    typedef uint16_t bigFieldAddress_t;
    typedef uint8_t fieldMapSize_t;
    typedef uint16_t debugOutputBufferSize_t;
    typedef uint8_t lockNumber_t;

    static const int maxParams = 10;

    static const bool findInterruptSourceAddress = true;
    static const int interruptNotificationQueueLength = 5;
    static const int scriptedNotificationQueueLength = 5;
    static const int maxInterruptScripts = 5;
    static const uint16_t maxScriptSize = 128;
    static const uint16_t scriptBigFieldSize = 16;
    static const uint8_t scriptChannelCount = 1;

    static const uint16_t maxParseIterations = 128;

    static const int scriptOutBufferSize = 32;
    static const uint16_t scriptOutReadBufferSize = 16;
    static const uint16_t interruptVectorOutReadBufferSize = 32;
    static const int maxLocks = 8;
    static const int debugBufferSize = 32;

    static const uint16_t serialBigSize = 32;
    static const uint16_t serialChannelReadBufferSize = 8;

    static const uint16_t mappingAddressCount = 32;

    static const uint8_t pinCount = 21;
};

#endif /* SRC_MAIN_CPP_ARDUINO_CONVERTER_FILES_ZSCRIPTDEFAULTPARAMETERS_HPP_ */
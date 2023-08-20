/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_FIRSTARDUINOTEST_ZSCRIPTPARAMETERS_HPP_
#define SRC_MAIN_CPP_FIRSTARDUINOTEST_ZSCRIPTPARAMETERS_HPP_

#define ZSCRIPT_IDENTIFY_USER_FIRMWARE_STRING         "Zscript Arduino Example"
#define ZSCRIPT_IDENTIFY_USER_FIRMWARE_VERSION        0x0001

#define ZSCRIPT_IDENTIFY_USER_HARDWARE_STRING     "Test setup"
#define ZSCRIPT_IDENTIFY_USER_HARDWARE_VERSION    0x0000

#include <ArduinoPlatformVersion.hpp>
#include <arduino/arduino-core-module/commands/ZscriptResetCommand.hpp>

#include <ZscriptIncludes.hpp>

// Please note that ZscriptFullInclude will set any #defines necessary to other set #defines - so ZSCRIPT_USE_DEBUG_ADDRESSING_SYSTEM enables ZSCRIPT_SUPPORT_DEBUG

//#define ZSCRIPT_SUPPORT_SCRIPT_SPACE
#define ZSCRIPT_SUPPORT_NOTIFICATIONS
#define ZSCRIPT_SUPPORT_ADDRESSING
//#define ZSCRIPT_DONT_FAST_DISCARD_COMMENTS

#define ZSCRIPT_HAVE_PIN_MODULE
//#define ZSCRIPT_HAVE_SERVO_MODULE
//#define ZSCRIPT_SERVO_MODULE_SLOW_MOVE
#define ZSCRIPT_HAVE_I2C_MODULE

#define ZSCRIPT_HAVE_UDP_CHANNEL
//#define ZSCRIPT_HAVE_TCP_CHANNEL
#define ZSCRIPT_HAVE_SERIAL_CHANNEL
//#define ZSCRIPT_HAVE_I2C_CHANNEL

#define I2C_ENABLE_GENERAL_CALL() (TWAR |= 1)
#define I2C_SET_ADDRESS(addr) (TWAR = addr<<1)

class ZscriptParams {
public:
    typedef Zscript::ZscriptResetCommand<ZscriptParams> ResetCommand;

    static uint16_t generateRandom16() {
        return (uint16_t) millis();
    }
    typedef uint8_t tokenBufferSize_t;

    static const uint8_t tcpChannelCount = 1;

    static const uint8_t udpChannelCount = 1;
    static const uint16_t udpLocalPort = 8888;
    static const uint8_t udpBufferSize = 128;

    static const uint8_t lockByteCount = 1;
    static const uint16_t serialBufferSize = 128;
    static const uint16_t i2cBufferSize = 128;

    static const uint16_t i2cChannelOutputBufferSize = 32;
    static const uint16_t i2cAlertPin = 4;
    static const uint8_t i2cChannelAddress = 0x61;

    static const uint16_t nonActivatedChannelTimeout = 5000;

    static const uint8_t pinCount = NUM_DIGITAL_PINS;

    static const uint8_t servoCount = 1;
    //static const uint8_t servoPins[1];
};
//const uint8_t ZscriptParams::servoPins[1] = {9};

#endif /* SRC_MAIN_CPP_FIRSTARDUINOTEST_ZSCRIPTPARAMETERS_HPP_ */

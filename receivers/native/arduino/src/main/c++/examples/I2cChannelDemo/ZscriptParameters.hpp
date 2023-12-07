/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_FIRSTARDUINOTEST_ZSCRIPTPARAMETERS_HPP_
#define SRC_MAIN_CPP_FIRSTARDUINOTEST_ZSCRIPTPARAMETERS_HPP_

#define ZSCRIPT_IDENTIFY_USER_FIRMWARE_STRING       "Zscript Arduino Example"
#define ZSCRIPT_IDENTIFY_USER_FIRMWARE_VERSION      0x0001

#define ZSCRIPT_IDENTIFY_USER_HARDWARE_STRING     "Test setup"
#define ZSCRIPT_IDENTIFY_USER_HARDWARE_VERSION    0x0000

#include <ArduinoPlatformVersion.hpp>
#include <arduino/arduino-core-module/commands/ZscriptResetCommand.hpp>
#include <arduino/arduino-core-module/commands/ZscriptIdCommand.hpp>

#include <ZscriptIncludes.hpp>

// Please note that ZscriptFullInclude will set any #defines necessary to other set #defines - eg ZSCRIPT_USE_DEBUG_ADDRESSING_SYSTEM enables ZSCRIPT_SUPPORT_DEBUG

//#define ZSCRIPT_SUPPORT_SCRIPT_SPACE
//#define ZSCRIPT_SUPPORT_NOTIFICATIONS
//#define ZSCRIPT_SUPPORT_ADDRESSING
//
#define ZSCRIPT_HAVE_PIN_MODULE
#define ZSCRIPT_PIN_SUPPORT_NOTIFICATIONS
//#define ZSCRIPT_PIN_SUPPORT_ANALOG_NOTIFICATIONS
//
#define ZSCRIPT_HAVE_UART_MODULE
#define ZSCRIPT_HAVE_UART_CHANNEL

//#define ZSCRIPT_HAVE_SERVO_MODULE
//#define ZSCRIPT_SERVO_MODULE_SLOW_MOVE

//#define ZSCRIPT_HAVE_UDP_CHANNEL
//#define ZSCRIPT_HAVE_TCP_CHANNEL

class ZscriptParams {
public:
    typedef Zscript::outer_core_module::ZscriptResetCommand<ZscriptParams> ResetCommand;
    typedef Zscript::core_module::ZscriptIdCommand<ZscriptParams> IdCommand;

    static uint16_t generateRandom16() {
        return (uint16_t) millis();
    }

    typedef uint8_t tokenBufferSize_t;
    static const uint8_t lockByteCount = 1;

    static const uint8_t pinCount = NUM_DIGITAL_PINS;

    static const uint16_t i2cBufferSize = 128;
    static const uint16_t i2cChannelOutputBufferSize = 32;
    static const uint16_t i2cAlertInPin = 4;
    static const uint16_t i2cAlertOutPin = 9;
    static const uint8_t i2cAddressingReadBlockLength = 8;
    static const uint8_t i2cChannelAddress = 0x61;

    static constexpr uint16_t uartCount = 1;
    static constexpr uint16_t uartRxBufferSize = 0;
    static constexpr uint16_t uartTxBufferSize = 0;
    static constexpr uint32_t uartSupportedFreqs[] = {9600, 115200};
    static constexpr uint16_t uartChannelInterface = 0;
    static constexpr uint16_t uartChannelBufferSize = 128;

    static constexpr uint8_t servoCount = 1;
    static constexpr uint8_t servoPins[] = { 9 };

    static constexpr uint8_t udpChannelCount = 1;
    static constexpr uint16_t udpLocalPort = 8888;
    static constexpr uint8_t udpBufferSize = 128;

    static constexpr uint8_t tcpChannelCount = 1;
    static constexpr uint16_t tcpLocalPort = 23;
    static constexpr uint8_t tcpBufferSize = 128;

    static constexpr uint16_t nonActivatedChannelTimeout = 5000;
};

constexpr uint32_t ZscriptParams::uartSupportedFreqs[];
constexpr uint8_t ZscriptParams::servoPins[1];

#endif /* SRC_MAIN_CPP_FIRSTARDUINOTEST_ZSCRIPTPARAMETERS_HPP_ */

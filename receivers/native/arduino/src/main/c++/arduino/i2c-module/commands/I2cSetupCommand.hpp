/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSETUPCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSETUPCOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "../I2cStrings.hpp"
#include <Wire.h>

#define COMMAND_EXISTS_0051 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptI2cSetupCommand {
public:
    static constexpr uint8_t CODE = 0x01;

    // if set, determines which port to control; else set all ports
    static constexpr char ParamPort__P = 'P';
    // chooses comms frequency, from 10, 100, 400 and 1000 kHz
    static constexpr char ParamFreq__F = 'F';
    // If present, sets whether addressing is enabled
    static constexpr char ParamAddressingEnable__A = 'A';
    // If present, sets whether notifications are enabled
    static constexpr char ParamNotificationEnable__N = 'N';

    static void execute(ZscriptCommandContext<ZP> ctx

#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
            , bool *addressing, bool *notifications
#endif
            ) {

        uint16_t freq;
        if (ctx.getField(ParamFreq__F, &freq)) {
            uint32_t freqValue;

            switch (freq) {
            case 0:
                freqValue = 10000;
                break;
            case 1:
                freqValue = 100000;
                break;
            case 2:
                freqValue = 400000;
                break;
            case 3:
                freqValue = 1000000;
                break;
            default:
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }

            uint16_t port;
            if (!ctx.getField(ParamPort__P, &port)) {
                ctx.status(ResponseStatus::MISSING_KEY);
                return;
            }
            if (port != 0) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
            Wire.setClock(freqValue);
        }
#ifdef ZSCRIPT_I2C_SUPPORT_NOTIFICATIONS
        uint16_t notif;
        if (ctx.getField(ParamNotificationEnable__N, &notif)) {
            *notifications = (notif != 0);
        }
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
        uint16_t addr;
        if (ctx.getField(ParamAddressingEnable__A, &addr)) {
            *addressing = (addr != 0);
        }
#endif
#endif
    }
};
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSETUPCOMMAND_HPP_ */

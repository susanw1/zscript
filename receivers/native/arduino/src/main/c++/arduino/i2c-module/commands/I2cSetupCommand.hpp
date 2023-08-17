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
    static constexpr char CMD_PARAM_I2C_PORT_P = 'P';

    // chooses comms frequency, from 10, 100, 400 and 1000 kHz
    static constexpr char CMD_PARAM_FREQ_F = 'F';

    static void execute(ZscriptCommandContext<ZP> ctx) {

        uint16_t freq;
        if (ctx.getField(CMD_PARAM_FREQ_F, &freq)) {
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
            if (!ctx.getField(CMD_PARAM_I2C_PORT_P, &port)) {
                ctx.status(ResponseStatus::MISSING_KEY);
                return;
            }
            if (port != 0) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
            Wire.setClock(freqValue);
        }
    }
};
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSETUPCOMMAND_HPP_ */

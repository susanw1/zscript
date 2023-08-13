/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSENDRECEIVECOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSENDRECEIVECOMMAND_HPP_

#include <zscript/modules/ZscriptCommand.hpp>
#include "GeneralI2cAction.hpp"
#include <Wire.h>
#define COMMAND_EXISTS_0054 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ZscriptI2cSendReceiveCommand {
public:
    static constexpr uint8_t CODE = 0x04;

    static void execute(ZscriptCommandContext<ZP> ctx) {
        GeneralI2cAction<ZP>::executeSendReceive(ctx, GeneralI2cAction<ZP>::ActionType::SEND_RECEIVE);
    }
};
}

#endif /* SRC_MAIN_CPP_ARDUINO_I2C_MODULE_COMMANDS_ZSCRIPTI2CSENDRECEIVECOMMAND_HPP_ */

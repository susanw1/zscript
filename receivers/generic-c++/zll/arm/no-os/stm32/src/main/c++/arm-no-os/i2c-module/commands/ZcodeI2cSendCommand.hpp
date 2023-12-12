/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include "GeneralI2cAction.hpp"

#define COMMAND_EXISTS_0052 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeI2cSendCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x02;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        GeneralI2cAction<ZP>::executeSendReceive(slot, GeneralI2cAction<ZP>::ActionType::SEND);
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CSENDCOMMAND_HPP_ */

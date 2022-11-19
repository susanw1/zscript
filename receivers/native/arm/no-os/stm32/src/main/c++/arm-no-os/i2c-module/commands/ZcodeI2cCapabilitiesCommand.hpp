/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CCAPABILITIESCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>

#define COMMAND_EXISTS_0050 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeI2cCapabilitiesCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x00;

    static constexpr char CMD_RESP_COMMANDS_C = 'C';
    static constexpr char CMD_RESP_NOTIFICATION_SUPPORTED_N = 'N';
    static constexpr char CMD_RESP_PORT_COUNT_P = 'P';
    static constexpr char CMD_RESP_FREQUENCIES_COUNT_F = 'F';

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        out->writeField8(CMD_RESP_COMMANDS_C, MODULE_CAPABILITIES(005));

        out->writeField8(CMD_RESP_NOTIFICATION_SUPPORTED_N, 0);

        out->writeField8(CMD_RESP_PORT_COUNT_P, ZP::LL::HW::i2cCount);
        out->writeField8(CMD_RESP_FREQUENCIES_COUNT_F, 4);
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_COMMANDS_ZCODEI2CCAPABILITIESCOMMAND_HPP_ */

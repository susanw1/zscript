/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_ZCODERESETCOMMAND_HPP_
#define SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_ZCODERESETCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>

#define COMMAND_EXISTS_0003 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeResetCommand: public ZcodeCommand<ZP> {

public:
    static constexpr uint8_t CODE = 0x03;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t target = 0;
        if (slot.getFields()->get('C', &target)) {
            if (target >= Zcode<ZP>::zcode.getChannelCount()) {
                slot.fail(BAD_PARAM, "Channel index too large");
                return;
            }
            if (!Zcode<ZP>::zcode.getChannel(target)->reset()) {
                slot.fail(CMD_FAIL, "Channel reset failed");
                return;
            }
        } else {
            void (*resetFunc)(void) = 0;
            resetFunc();
        }
        out->writeStatus(OK);
    }

};

#define ZCODE_CORE_RESET_COMMAND ZcodeResetCommand<ZP>::execute

#endif /* SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_ZCODERESETCOMMAND_HPP_ */

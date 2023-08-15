/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEMATCHCODECOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEMATCHCODECOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"
#include "ZcodeMakeCodeCommand.hpp"

#define COMMAND_EXISTS_000D EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeMatchCodeCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x0d;
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t value = 0;
        if (slot.getFields()->get('C', &value)) {
            if (value == ZcodeMakeCodeCommand<ZP>::number) {
                out->writeStatus(OK);
            } else {
                slot.fail(CMD_FAIL, (string_t) NULL);
            }
        } else {
            slot.fail(BAD_PARAM, (string_t) ZP::Strings::failMatchCodeNoCodeGiven);
        }
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEMATCHCODECOMMAND_HPP_ */

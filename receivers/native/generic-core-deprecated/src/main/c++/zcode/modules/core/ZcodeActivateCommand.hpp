/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEACTIVATECOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEACTIVATECOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_EXISTS_0002 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeActivateCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x02;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField8('A', (uint8_t) slot.getZcode()->isActivated());
        out->writeStatus(OK);
        slot.getZcode()->setActivated();
    }

};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEACTIVATECOMMAND_HPP_ */

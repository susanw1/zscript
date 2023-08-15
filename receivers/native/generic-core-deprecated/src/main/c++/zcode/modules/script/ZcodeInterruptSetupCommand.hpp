/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODEINTERRUPTSETUPCOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODEINTERRUPTSETUPCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_EXISTS_0028 EXISTENCE_MARKER_UTIL
template<class ZP>
class ZcodeOutStream;
template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeInterruptSetupCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x08;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
//        ZcodeScriptSpace<ZP> *space = slot.getZcode()->getSpace();

        out->writeField8('I', ZP::maxInterruptScripts);
        out->writeStatus(OK);

    }

};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODEINTERRUPTSETUPCOMMAND_HPP_ */

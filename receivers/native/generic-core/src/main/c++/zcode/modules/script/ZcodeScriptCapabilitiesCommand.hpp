/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTCAPABILITIESCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_EXISTS_0020 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeOutStream;
template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeScriptCapabilitiesCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x00;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        ZcodeScriptSpace<ZP> *space = slot.getZcode()->getSpace();

        out->writeField8('D', space->getDelay());
        if (space->hasFailed()) {
            out->writeField8('F', (uint8_t) 0);
        }
        out->writeField8('G', (uint8_t) space->isRunning());
        out->writeField16('L', space->getLength());
        out->writeField32('M', ZP::maxScriptSize);
        out->writeField16('C', MODULE_CAPABILITIES(001));
        out->writeStatus(OK);

    }

};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTCAPABILITIESCOMMAND_HPP_ */

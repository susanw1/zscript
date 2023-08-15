/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTSTORECOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTSTORECOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_EXISTS_0021 EXISTENCE_MARKER_UTIL
template<class ZP>
class ZcodeOutStream;
template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeScriptStoreCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x01;
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        ZcodeScriptSpace<ZP> *space = slot.getZcode()->getSpace();

        uint16_t address = 0;
        bool valid = true;
        if (valid && slot.getFields()->get('A', &address)) {
            if (address >= space->getLength()) {
                valid = false;
                slot.fail(BAD_PARAM, (string_t) ZP::Strings::failScriptBadAddress);
            }
        }
        if (valid && address + slot.getBigField()->getLength() < ZP::maxScriptSize) {
            space->write(slot.getBigField()->getData(), slot.getBigField()->getLength(), address, slot.getFields()->has('L'));
            out->writeStatus(OK);
        } else {
            slot.fail(BAD_PARAM, (string_t) ZP::Strings::failScriptBadWrite);
        }
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTSTORECOMMAND_HPP_ */

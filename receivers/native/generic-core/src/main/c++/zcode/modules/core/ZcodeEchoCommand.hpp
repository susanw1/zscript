/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEECHOCOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEECHOCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_EXISTS_0001 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeEchoCommand: public ZcodeCommand<ZP> {
private:
    typedef typename ZP::Strings::string_t string_t;
    static const uint8_t CODE = 0x01;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t statusResult;
        if (!slot.getFields()->get(Zchars::STATUS_RESP_PARAM, &statusResult)) {
            out->writeStatus(OK);
        } else {
            slot.fail((ZcodeResponseStatus) statusResult, (string_t) NULL);
        }
        slot.getFields()->copyTo(out);
        slot.getBigField()->copyTo(out);
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_CORE_ZCODEECHOCOMMAND_HPP_ */

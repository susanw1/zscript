/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_RUNNING_ZCODECOMMANDFINDER_HPP_
#define SRC_MAIN_CPP_ZCODE_RUNNING_ZCODECOMMANDFINDER_HPP_

#include "../ZcodeIncludes.hpp"
#include "../modules/ZcodeModule.hpp"

#define MODULE_SWITCH_CHECK(x, y) MODULE_EXISTS_##x##y
#define MODULE_SWITCH_SWITCH(x, y) MODULE_SWITCH_##x##y break;

#define MODULE_SWITCH() MODULE_SWITCHING_GENERIC(MODULE_SWITCH_CHECK, MODULE_SWITCH_SWITCH)

template<class ZP>
class ZcodeRunningCommandSlot;

template<class ZP>
class ZcodeCommandFinder {
    typedef typename ZP::Strings::string_t string_t;

public:
    static const uint16_t MAX_SYSTEM_CODE = 15;

    static void runCommand(ZcodeRunningCommandSlot<ZP> *outerSlot) {
        if (outerSlot->failedParse()) {
            outerSlot->performFail();
            return;
        }
        ZcodeExecutionCommandSlot<ZP> slot(outerSlot);
        if (outerSlot->isEmpty()) {
            slot.setComplete();
            slot.getOut()->silentSucceed();
            return;
        }

        uint16_t echoValue;
        if (slot.getFields()->get(Zchars::ECHO_PARAM, &echoValue)) {
            slot.getOut()->writeField16(Zchars::ECHO_PARAM, echoValue);
        }

        uint16_t command;
        if (!slot.getFields()->get(Zchars::CMD_PARAM, &command)) {
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseNoZ);
            return;
        }

        if (command > MAX_SYSTEM_CODE && !slot.getZcode()->isActivated()) {
            slot.fail(NOT_ACTIVATED, (string_t) ZP::Strings::failParseNotActivated);
            return;
        }

        // set "complete" as a default assumption, but commands may unset it if they perform async activity.
        slot.setComplete();

        uint8_t commandBottomBits = (uint8_t) (command & 0xF);
        (void) commandBottomBits;

        switch (command >> 4) {
        MODULE_SWITCH()

    default:
        slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
        break;
        }
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_RUNNING_ZCODECOMMANDFINDER_HPP_ */

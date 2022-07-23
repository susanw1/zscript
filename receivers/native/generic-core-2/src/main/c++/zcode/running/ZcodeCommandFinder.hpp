/*
 * ZcodeCommandFinder.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_
#include "../ZcodeIncludes.hpp"
#include "../modules/ZcodeModule.hpp"

#define MODULE_SWITCH_CHECK(v) MODULE_EXISTS_##v
#define MODULE_SWITCH_SWITCH(v) MODULE_SWITCH_##v break;

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
        uint16_t command;
        if (!slot.getFields()->get('Z', &command)) {
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseNoZ);
            return;
        }
        if (command > MAX_SYSTEM_CODE && !slot.getZcode()->isActivated()) {
            slot.fail(NOT_ACTIVATED, (string_t) ZP::Strings::failParseNotActivated);
            return;
        }
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

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_ */

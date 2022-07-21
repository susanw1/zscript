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

#define COMMAND_SWITCH3(x, y, z) COMMAND_SWITCH_TAKE_IF_DEFV(COMMAND_SWITCH##x##y##z, ;)

#define COMMAND_SWITCH2_EACH(x, y) COMMAND_SWITCH3(x, y, 0) COMMAND_SWITCH3(x, y, 1) COMMAND_SWITCH3(x, y, 2) COMMAND_SWITCH3(x, y, 3) \
                                COMMAND_SWITCH3(x, y, 4) COMMAND_SWITCH3(x, y, 5) COMMAND_SWITCH3(x, y, 6) COMMAND_SWITCH3(x, y, 7) \
                                COMMAND_SWITCH3(x, y, 8) COMMAND_SWITCH3(x, y, 9) COMMAND_SWITCH3(x, y, A) COMMAND_SWITCH3(x, y, B) \
                                COMMAND_SWITCH3(x, y, C) COMMAND_SWITCH3(x, y, D) COMMAND_SWITCH3(x, y, E) COMMAND_SWITCH3(x, y, F)

#define COMMAND_SWITCH2(x, y) COMMAND_SWITCH2_EACH(x, y)

#define COMMAND_SWITCH1_EACH(x) COMMAND_SWITCH2(x, 0) COMMAND_SWITCH2(x, 1) COMMAND_SWITCH2(x, 2) COMMAND_SWITCH2(x, 3) \
                                COMMAND_SWITCH2(x, 4) COMMAND_SWITCH2(x, 5) COMMAND_SWITCH2(x, 6) COMMAND_SWITCH2(x, 7) \
                                COMMAND_SWITCH2(x, 8) COMMAND_SWITCH2(x, 9) COMMAND_SWITCH2(x, A) COMMAND_SWITCH2(x, B) \
                                COMMAND_SWITCH2(x, C) COMMAND_SWITCH2(x, D) COMMAND_SWITCH2(x, E) COMMAND_SWITCH2(x, F)

#define COMMAND_SWITCH1(x) COMMAND_SWITCH1_EACH(x)

#define COMMAND_SWITCH0_EACH() COMMAND_SWITCH1(0) COMMAND_SWITCH1(1) COMMAND_SWITCH1(2) COMMAND_SWITCH1(3) \
                                COMMAND_SWITCH1(4) COMMAND_SWITCH1(5) COMMAND_SWITCH1(6) COMMAND_SWITCH1(7) \
                                COMMAND_SWITCH1(8) COMMAND_SWITCH1(9) COMMAND_SWITCH1(A) COMMAND_SWITCH1(B) \
                                COMMAND_SWITCH1(C) COMMAND_SWITCH1(D) COMMAND_SWITCH1(E) COMMAND_SWITCH1(F)

#define COMMAND_SWITCH0() COMMAND_SWITCH0_EACH()

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
        if (outerSlot->isEmpty()) {
            outerSlot->setComplete();
            return;
        }
        uint16_t command;
        if (!outerSlot->getFields()->get('Z', &command)) {
            outerSlot->fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseNoZ);
            return;
        }
        if (command > MAX_SYSTEM_CODE && !outerSlot->getZcode()->isActivated()) {
            outerSlot->fail(NOT_ACTIVATED, (string_t) ZP::Strings::failParseNotActivated);
            return;
        }
        outerSlot->setComplete();
        ZcodeExecutionCommandSlot<ZP> slot(outerSlot);
        uint8_t commandBottomBits = (uint8_t) (command & 0xF);
        switch (command >> 4) {
        COMMAND_SWITCH0()

    default:
        slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
        break;
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_ */

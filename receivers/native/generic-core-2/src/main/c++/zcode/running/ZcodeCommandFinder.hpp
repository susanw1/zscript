/*
 * ZcodeCommandFinder.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_
#include "../ZcodeIncludes.hpp"
#include "../modules/core/ZcodeActivateCommand.hpp"

#define MACROADDCA3 F
#define MACROADDCA2 E
#define MACROADDCA1 D
#define MACROADDCA0 C

#define MACROADD8A3 B
#define MACROADD8A2 A
#define MACROADD8A1 9
#define MACROADD8A0 8

#define MACROADD4A3 7
#define MACROADD4A2 6
#define MACROADD4A1 5
#define MACROADD4A0 4

#define MACROADD0A3 3
#define MACROADD0A2 2
#define MACROADD0A1 1
#define MACROADD0A0 0

#define COMMAND_SWITCH_TAKE_IF_DEF(thing, x, ...) x
#define COMMAND_SWITCH_TAKE_IF_DEFI(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEF(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFII(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEF(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFIII(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFII(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFIV(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFIII(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFV(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFIV(thing, x, __VA_ARG__)

#define COMMAND_SWITCH3(x, y, z) COMMAND_SWITCH_TAKE_IF_DEFV(COMMAND_SWITCH##x##y##z, ;)
#define COMMAND_SWITCH3I(x, y, a) COMMAND_SWITCH3(x, y, a)

#define COMMAND_SWITCH2_EACH1(x, y, a) COMMAND_SWITCH3I(x, y, MACROADD##a##A0) COMMAND_SWITCH3I(x, y, MACROADD##a##A1) COMMAND_SWITCH3I(x, y, MACROADD##a##A2) COMMAND_SWITCH3I(x, y, MACROADD##a##A3)

#define COMMAND_SWITCH2_EACH(x, y) COMMAND_SWITCH2_EACH1(x, y, 0) COMMAND_SWITCH2_EACH1(x, y, 4) COMMAND_SWITCH2_EACH1(x, y, 8) COMMAND_SWITCH2_EACH1(x, y, C)

#define COMMAND_SWITCH2(x, y) COMMAND_SWITCH2_EACH(x, y)
#define COMMAND_SWITCH2I(x, y) COMMAND_SWITCH2(x, y)

#define COMMAND_SWITCH1_EACH1(x, a) COMMAND_SWITCH2I(x, MACROADD##a##A0) COMMAND_SWITCH2I(x, MACROADD##a##A1) COMMAND_SWITCH2I(x, MACROADD##a##A2) COMMAND_SWITCH2I(x, MACROADD##a##A3)

#define COMMAND_SWITCH1_EACH(x) COMMAND_SWITCH1_EACH1(x, 0) COMMAND_SWITCH1_EACH1(x, 4) COMMAND_SWITCH1_EACH1(x, 8) COMMAND_SWITCH1_EACH1(x, C)

#define COMMAND_SWITCH1(x) COMMAND_SWITCH1_EACH(x)
#define COMMAND_SWITCH1I(x) COMMAND_SWITCH1(x)

#define COMMAND_SWITCH0_EACH1(a) COMMAND_SWITCH1I(MACROADD##a##A0) COMMAND_SWITCH1I(MACROADD##a##A1) COMMAND_SWITCH1I(MACROADD##a##A2) COMMAND_SWITCH1I(MACROADD##a##A3)

#define COMMAND_SWITCH0_EACH() COMMAND_SWITCH0_EACH1(0) COMMAND_SWITCH0_EACH1(4) COMMAND_SWITCH0_EACH1(8) COMMAND_SWITCH0_EACH1(C)

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
        switch (command >> 4) {
        COMMAND_SWITCH0()

    default:
        slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
        break;
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDFINDER_HPP_ */

/*
 * ZcodeModule.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEMODULE_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEMODULE_HPP_

#include "../ZcodeIncludes.hpp"
#include "../running/ZcodeExecutionCommandSlot.hpp"

#define COMMAND_SWITCH_UTIL(num, func) 0, case num: func(slot, command); break;

//#define COMMAND_SWITCH000 COMMAND_SWITCH_UTIL(0x0, exec)
//#define COMMAND_SWITCH03A COMMAND_SWITCH_UTIL(0x3a, exec2)
template<class ZP>
class ZcodeModule {

public:
    const uint16_t moduleId;

    ZcodeModule(uint16_t moduleId) :
            moduleId(moduleId) {

    }
};
// want function of form: void execute(ZcodeExecutionCommandSlot<ZP> slot, uint16_t command)

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEMODULE_HPP_ */

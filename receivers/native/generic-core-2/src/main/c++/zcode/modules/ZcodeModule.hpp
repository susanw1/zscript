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
#include "../addressing/ZcodeAddressRouter.hpp"

#define COMMAND_SWITCH_UTIL(num, func) 0, case num: func(slot, command); break;

//#define COMMAND_SWITCH000 COMMAND_SWITCH_UTIL(0x0, exec)
//#define COMMAND_SWITCH03A COMMAND_SWITCH_UTIL(0x3a, exec2)
template<class ZP>
class ZcodeModule {

public:
    const uint16_t moduleId;

#ifdef ZCODE_SUPPORT_ADDRESSING
    ZcodeAddressRouter<ZP> *addressRouter;
    ZcodeModule(uint16_t moduleId, ZcodeAddressRouter<ZP> *addressRouter) :
            moduleId(moduleId), addressRouter(addressRouter) {
    }
#endif

    ZcodeModule(uint16_t moduleId) :
            moduleId(moduleId)

#ifdef ZCODE_SUPPORT_ADDRESSING
    , addressRouter(NULL)
#endif
    {
    }
};
// want function of form: void execute(ZcodeExecutionCommandSlot<ZP> slot, uint16_t command)

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEMODULE_HPP_ */

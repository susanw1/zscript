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

#define COMMAND_SWITCH_TAKE_IF_SECOND_DEF(thing, otherThing, x, ...) x
#define COMMAND_SWITCH_TAKE_IF_SECOND_DEFI(thing, otherThing, x, ...) COMMAND_SWITCH_TAKE_IF_SECOND_DEF(thing, otherThing, x, ...)
#define COMMAND_SWITCH_TAKE_IF_SECOND_DEFII(thing, otherThing, x, ...) COMMAND_SWITCH_TAKE_IF_SECOND_DEFI(thing, otherThing, x, ...)
#define COMMAND_SWITCH_TAKE_IF_SECOND_DEFIII(thing, otherThing, x, ...) COMMAND_SWITCH_TAKE_IF_SECOND_DEFII(thing, otherThing, x, ...)
#define COMMAND_SWITCH_TAKE_IF_SECOND_DEFIV(thing, otherThing, x, ...) COMMAND_SWITCH_TAKE_IF_SECOND_DEFIII(thing, otherThing, x, ...)
#define COMMAND_SWITCH_TAKE_IF_SECOND_DEFV(thing, otherThing, x, ...) COMMAND_SWITCH_TAKE_IF_SECOND_DEFIV(thing, otherThing, x, ...)

#define MODULE_CAPABILITIESInternal(value, number) COMMAND_SWITCH_TAKE_IF_SECOND_DEFV(COMMAND_VALUE_##value, (1 << number), 0)
#define MODULE_CAPABILITIES(value) (MODULE_CAPABILITIESInternal(value##0, 0) | MODULE_CAPABILITIESInternal(value##1, 1) | MODULE_CAPABILITIESInternal(value##2, 2) | MODULE_CAPABILITIESInternal(value##3, 3) | \
        MODULE_CAPABILITIESInternal(value##4, 4) | MODULE_CAPABILITIESInternal(value##5, 5) | MODULE_CAPABILITIESInternal(value##6, 6) | MODULE_CAPABILITIESInternal(value##7, 7) | \
        MODULE_CAPABILITIESInternal(value##8, 8) | MODULE_CAPABILITIESInternal(value##9, 9) | MODULE_CAPABILITIESInternal(value##A, 10) | MODULE_CAPABILITIESInternal(value##B, 11) | \
        MODULE_CAPABILITIESInternal(value##C, 12) | MODULE_CAPABILITIESInternal(value##D, 13) | MODULE_CAPABILITIESInternal(value##E, 14) | MODULE_CAPABILITIESInternal(value##F, 15))

#define COMMAND_SWITCH_UTIL(num, func) 0, case num: func(slot, commandBottomBits); break;

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

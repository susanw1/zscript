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

#define COMMAND_SWITCH_TAKE_IF_DEF(thing, x, ...) x
#define COMMAND_SWITCH_TAKE_IF_DEFI(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEF(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFII(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEF(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFIII(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFII(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFIV(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFIII(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFV(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFIV(thing, x, __VA_ARG__)

#define MODULE_CAPABILITIESInternal(value, number) COMMAND_SWITCH_TAKE_IF_SECOND_DEFV(COMMAND_VALUE_##value, (1 << number), 0)
#define MODULE_CAPABILITIES(value) (MODULE_CAPABILITIESInternal(value##0, 0) | MODULE_CAPABILITIESInternal(value##1, 1) | MODULE_CAPABILITIESInternal(value##2, 2) | MODULE_CAPABILITIESInternal(value##3, 3) | \
        MODULE_CAPABILITIESInternal(value##4, 4) | MODULE_CAPABILITIESInternal(value##5, 5) | MODULE_CAPABILITIESInternal(value##6, 6) | MODULE_CAPABILITIESInternal(value##7, 7) | \
        MODULE_CAPABILITIESInternal(value##8, 8) | MODULE_CAPABILITIESInternal(value##9, 9) | MODULE_CAPABILITIESInternal(value##A, 10) | MODULE_CAPABILITIESInternal(value##B, 11) | \
        MODULE_CAPABILITIESInternal(value##C, 12) | MODULE_CAPABILITIESInternal(value##D, 13) | MODULE_CAPABILITIESInternal(value##E, 14) | MODULE_CAPABILITIESInternal(value##F, 15))

#define COMMAND_SWITCH_UTIL(num, func) 0, case num: func(slot, commandBottomBits); break;

#define MODULE_ADDRESS_UTIL 0, true

//#define COMMAND_SWITCH000 COMMAND_SWITCH_UTIL(0x0, exec)
//#define COMMAND_SWITCH03A COMMAND_SWITCH_UTIL(0x3a, exec2)
#define COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(module, number) ((COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS00##module, false)?1:0) << number)

#define COMMAND_SWITCH_EXISTS_BOTTOM_BYTE (COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(0, 0) | COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(1, 1) | \
        COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(2, 2) | COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(3, 3) | \
        COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(4, 4) | COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(5, 5) | \
        COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(6, 6) | COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(7, 7) | \
        COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(8, 8) | COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(9, 9) | \
        COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(A, 10) | COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(B, 11) | \
        COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(C, 12) | COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(D, 13) | \
        COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(E, 14) | COMMAND_SWITCH_EXISTS_BOTTOM_BYTE_PER_MODULE(F, 15))

#define COMMAND_SWITCH_EXISTS_2_FIGS(prefix) (COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##0, false) || COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##1, false) || \
        COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##2, false) || COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##3, false) || \
        COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##4, false) || COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##5, false) || \
        COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##6, false) || COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##7, false) || \
        COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##8, false) || COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##9, false) || \
        COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##A, false) || COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##B, false) || \
        COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##C, false) || COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##D, false) || \
        COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##E, false) || COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_ADDRESS##prefix##F, false))

#define COMMAND_SWITCH_EXISTS_2_FIGS_Internal(prefix, number) ((COMMAND_SWITCH_EXISTS_2_FIGS(prefix) ? 1 : 0) << number)

#define COMMAND_SWITCH_BYTE(value) (COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##0, 0) | COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##1, 1) | \
        COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##2, 2) | COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##3, 3) | \
        COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##4, 4) | COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##5, 5) | \
        COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##6, 6) | COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##7, 7) | \
        COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##8, 8) | COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##9, 9) | \
        COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##A, 10) | COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##B, 11) | \
        COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##C, 12) | COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##D, 13) | \
        COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##E, 14) | COMMAND_SWITCH_EXISTS_2_FIGS_Internal(value##F, 15))

#define COMMAND_SWITCH_BYTE_EXISTS(value) (COMMAND_SWITCH_EXISTS_2_FIGS(value##0) || COMMAND_SWITCH_EXISTS_2_FIGS(value##1) || \
        COMMAND_SWITCH_EXISTS_2_FIGS(value##2) || COMMAND_SWITCH_EXISTS_2_FIGS(value##3) || \
        COMMAND_SWITCH_EXISTS_2_FIGS(value##4) || COMMAND_SWITCH_EXISTS_2_FIGS(value##5) || \
        COMMAND_SWITCH_EXISTS_2_FIGS(value##6) || COMMAND_SWITCH_EXISTS_2_FIGS(value##7) || \
        COMMAND_SWITCH_EXISTS_2_FIGS(value##8) || COMMAND_SWITCH_EXISTS_2_FIGS(value##9) || \
        COMMAND_SWITCH_EXISTS_2_FIGS(value##A) || COMMAND_SWITCH_EXISTS_2_FIGS(value##B) || \
        COMMAND_SWITCH_EXISTS_2_FIGS(value##C) || COMMAND_SWITCH_EXISTS_2_FIGS(value##D) || \
        COMMAND_SWITCH_EXISTS_2_FIGS(value##E) || COMMAND_SWITCH_EXISTS_2_FIGS(value##F))

#define BROAD_MODULE_EXISTENCE_Internal(value, number) ((COMMAND_SWITCH_BYTE_EXISTS(value) ? 1 : 0) << number)

#define BROAD_MODULE_EXISTENCE (BROAD_MODULE_EXISTENCE_Internal(0, 0) | BROAD_MODULE_EXISTENCE_Internal(1, 1) | \
        BROAD_MODULE_EXISTENCE_Internal(2, 2) | BROAD_MODULE_EXISTENCE_Internal(3, 3) | \
        BROAD_MODULE_EXISTENCE_Internal(4, 4) | BROAD_MODULE_EXISTENCE_Internal(5, 5) | \
        BROAD_MODULE_EXISTENCE_Internal(6, 6) | BROAD_MODULE_EXISTENCE_Internal(7, 7) | \
        BROAD_MODULE_EXISTENCE_Internal(8, 8) | BROAD_MODULE_EXISTENCE_Internal(9, 9) | \
        BROAD_MODULE_EXISTENCE_Internal(A, 10) | BROAD_MODULE_EXISTENCE_Internal(B, 11) | \
        BROAD_MODULE_EXISTENCE_Internal(C, 12) | BROAD_MODULE_EXISTENCE_Internal(D, 13) | \
        BROAD_MODULE_EXISTENCE_Internal(E, 14) | BROAD_MODULE_EXISTENCE_Internal(F, 15))

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

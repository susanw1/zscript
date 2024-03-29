/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_ZSCRIPTMODULE_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_ZSCRIPTMODULE_HPP_

#include "../ZscriptIncludes.hpp"

#define COMMAND_SWITCH_TAKE_IF_DEF(thing, x, ...) x
#define COMMAND_SWITCH_TAKE_IF_DEFI(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEF(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFII(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEF(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFIII(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFII(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFIV(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFIII(thing, x, __VA_ARG__)
#define COMMAND_SWITCH_TAKE_IF_DEFV(thing, x, ...) COMMAND_SWITCH_TAKE_IF_DEFIV(thing, x, __VA_ARG__)

#define MODULE_SWITCH_UTIL(func) func(ctx, commandBottomBits);
#define MODULE_SWITCH_UTIL_WITH_ASYNC(func) func(ctx, commandBottomBits, moveAlong);
#define ADDRESS_SWITCH_UTIL(func) func(ctx);
#define ADDRESS_SWITCH_UTIL_WITH_ASYNC(func) func(ctx, moveAlong);
#define NOTIFICATION_SWITCH_UTIL(func) func(ctx, moveAlong);

#define EXISTENCE_MARKER_UTIL 0,

#define NIBBLE_BIT_SHIFT_GENERIC(func, prefix) (func(prefix##0, 0) | func(prefix##1, 1) | \
        func(prefix##2, 2) | func(prefix##3, 3) | \
        func(prefix##4, 4) | func(prefix##5, 5) | \
        func(prefix##6, 6) | func(prefix##7, 7) | \
        func(prefix##8, 8) | func(prefix##9, 9) | \
        func(prefix##a, 10) | func(prefix##b, 11) | \
        func(prefix##c, 12) | func(prefix##d, 13) | \
        func(prefix##e, 14) | func(prefix##f, 15))

#define NIBBLE_CHECK_GENERIC(func, prefix) (func(prefix##0) || func(prefix##1) ||\
        func(prefix##2) || func(prefix##3) || \
        func(prefix##4) || func(prefix##5) || \
        func(prefix##6) || func(prefix##7) || \
        func(prefix##8) || func(prefix##9) || \
        func(prefix##a) || func(prefix##b) || \
        func(prefix##c) || func(prefix##d) || \
        func(prefix##e) || func(prefix##f))

#define NIBBLE_CHECK_GENERICI(func, prefix) (func(prefix##0) || func(prefix##1) ||\
        func(prefix##2) || func(prefix##3) || \
        func(prefix##4) || func(prefix##5) || \
        func(prefix##6) || func(prefix##7) || \
        func(prefix##8) || func(prefix##9) || \
        func(prefix##a) || func(prefix##b) || \
        func(prefix##c) || func(prefix##d) || \
        func(prefix##e) || func(prefix##f))
//
//
//

#define MODULE_CAPABILITIESInternal(value, number) COMMAND_SWITCH_TAKE_IF_DEFV(COMMAND_EXISTS_##value (1 << number), 0)
//

#define MODULE_CAPABILITIES(value) NIBBLE_BIT_SHIFT_GENERIC(MODULE_CAPABILITIESInternal, value)
//
//

#define COMMAND_SWITCH_EXISTS_PER_MODULE(module) COMMAND_SWITCH_TAKE_IF_DEFV(MODULE_EXISTS_##module true, false)
//

#define COMMAND_SWITCH_BITS_PER_MODULE(module, number) ((COMMAND_SWITCH_EXISTS_PER_MODULE(module) ? 1 : 0) << number)
//

#define COMMAND_SWITCH_EXISTS_BOTTOM_BYTE(prefix) NIBBLE_BIT_SHIFT_GENERIC(COMMAND_SWITCH_BITS_PER_MODULE, prefix)
//

#define COMMAND_SWITCH_EXISTS_2_FIGS(prefix) NIBBLE_CHECK_GENERICI(COMMAND_SWITCH_EXISTS_PER_MODULE, prefix)
//

#define COMMAND_SWITCH_EXISTS_2_FIGS_Internal(prefix, number) ((COMMAND_SWITCH_EXISTS_2_FIGS(prefix) ? 1 : 0) << number)
//

#define COMMAND_SWITCH_BYTE(value) NIBBLE_BIT_SHIFT_GENERIC(COMMAND_SWITCH_EXISTS_2_FIGS_Internal, value)
//

#define COMMAND_SWITCH_BYTE_EXISTS(value) (NIBBLE_CHECK_GENERIC(COMMAND_SWITCH_EXISTS_2_FIGS, value))
//

#define BROAD_MODULE_EXISTENCE_Internal(value, number) ((COMMAND_SWITCH_BYTE_EXISTS(value) ? 1 : 0) << number)
//

#define BROAD_MODULE_EXISTENCE NIBBLE_BIT_SHIFT_GENERIC(BROAD_MODULE_EXISTENCE_Internal, )
//
//
//
//
#define MODULE_SWITCHING_GENERIC3(check, sw, x, y) COMMAND_SWITCH_TAKE_IF_DEFV(check(x, y) case 0x##x##y: sw(x, y), ;)

#define MODULE_SWITCHING_GENERIC2_EACH(check, sw, x) MODULE_SWITCHING_GENERIC3(check, sw, x, 0) MODULE_SWITCHING_GENERIC3(check, sw, x, 1) \
        MODULE_SWITCHING_GENERIC3(check, sw, x, 2) MODULE_SWITCHING_GENERIC3(check, sw, x, 3) \
        MODULE_SWITCHING_GENERIC3(check, sw, x, 4) MODULE_SWITCHING_GENERIC3(check, sw, x, 5) \
        MODULE_SWITCHING_GENERIC3(check, sw, x, 6) MODULE_SWITCHING_GENERIC3(check, sw, x, 7) \
        MODULE_SWITCHING_GENERIC3(check, sw, x, 8) MODULE_SWITCHING_GENERIC3(check, sw, x, 9) \
        MODULE_SWITCHING_GENERIC3(check, sw, x, a) MODULE_SWITCHING_GENERIC3(check, sw, x, b) \
        MODULE_SWITCHING_GENERIC3(check, sw, x, c) MODULE_SWITCHING_GENERIC3(check, sw, x, d) \
        MODULE_SWITCHING_GENERIC3(check, sw, x, e) MODULE_SWITCHING_GENERIC3(check, sw, x, f)

#define MODULE_SWITCHING_GENERIC2(check, sw, x) MODULE_SWITCHING_GENERIC2_EACH(check, sw, x)

#define MODULE_SWITCHING_GENERIC1_EACH(check, sw, x) MODULE_SWITCHING_GENERIC2(check, sw, x##0) MODULE_SWITCHING_GENERIC2(check, sw, x##1) \
        MODULE_SWITCHING_GENERIC2(check, sw, x##2) MODULE_SWITCHING_GENERIC2(check, sw, x##3) \
        MODULE_SWITCHING_GENERIC2(check, sw, x##4) MODULE_SWITCHING_GENERIC2(check, sw, x##5) \
        MODULE_SWITCHING_GENERIC2(check, sw, x##6) MODULE_SWITCHING_GENERIC2(check, sw, x##7) \
        MODULE_SWITCHING_GENERIC2(check, sw, x##8) MODULE_SWITCHING_GENERIC2(check, sw, x##9) \
        MODULE_SWITCHING_GENERIC2(check, sw, x##a) MODULE_SWITCHING_GENERIC2(check, sw, x##b) \
        MODULE_SWITCHING_GENERIC2(check, sw, x##c) MODULE_SWITCHING_GENERIC2(check, sw, x##d) \
        MODULE_SWITCHING_GENERIC2(check, sw, x##e) MODULE_SWITCHING_GENERIC2(check, sw, x##f)

#define MODULE_SWITCHING_GENERIC1(check, sw, x) MODULE_SWITCHING_GENERIC1_EACH(check, sw, x)

#define MODULE_SWITCHING_GENERIC_EACH(check, sw) MODULE_SWITCHING_GENERIC1(check, sw, 0) MODULE_SWITCHING_GENERIC1(check, sw, 1) \
        MODULE_SWITCHING_GENERIC1(check, sw, 2) MODULE_SWITCHING_GENERIC1(check, sw, 3) \
        MODULE_SWITCHING_GENERIC1(check, sw, 4) MODULE_SWITCHING_GENERIC1(check, sw, 5) \
        MODULE_SWITCHING_GENERIC1(check, sw, 6) MODULE_SWITCHING_GENERIC1(check, sw, 7) \
        MODULE_SWITCHING_GENERIC1(check, sw, 8) MODULE_SWITCHING_GENERIC1(check, sw, 9) \
        MODULE_SWITCHING_GENERIC1(check, sw, a) MODULE_SWITCHING_GENERIC1(check, sw, b) \
        MODULE_SWITCHING_GENERIC1(check, sw, c) MODULE_SWITCHING_GENERIC1(check, sw, d) \
        MODULE_SWITCHING_GENERIC1(check, sw, e) MODULE_SWITCHING_GENERIC1(check, sw, f)

#define MODULE_SWITCHING_GENERIC(check, sw) MODULE_SWITCHING_GENERIC_EACH(check, sw)

template<class ZP>
class ZscriptModule {

public:

};

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_ZSCRIPTMODULE_HPP_ */

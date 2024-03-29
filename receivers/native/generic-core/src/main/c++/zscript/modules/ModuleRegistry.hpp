/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_MODULEREGISTRY_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_MODULEREGISTRY_HPP_
#include "../ZscriptIncludes.hpp"
#ifdef ZSCRIPT_SUPPORT_ADDRESSING
#include "../execution/ZscriptAddressingContext.hpp"
#endif
#include "../execution/ZscriptCommandContext.hpp"
#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
#include "../execution/ZscriptNotificationContext.hpp"
#endif
#include "ZscriptModule.hpp"

#define MODULE_SWITCH_CHECK(x, y) MODULE_EXISTS_##x##y
#define MODULE_SWITCH_SWITCH(x, y) MODULE_SWITCH_##x##y break;

#define ADDRESS_SWITCH_CHECK(x, y) MODULE_ADDRESS_EXISTS_##x##y
#define ADDRESS_SWITCH_SWTICH(x, y) MODULE_ADDRESS_SWITCH_##x##y break;

#define NOTIFICATION_SWITCH_CHECK(x, y) MODULE_NOTIFICATION_EXISTS_##x##y
#define NOTIFICATION_SWITCH_SWTICH(x, y) MODULE_NOTIFICATION_SWITCH_##x##y break;

#define MODULE_SWITCH() MODULE_SWITCHING_GENERIC(MODULE_SWITCH_CHECK, MODULE_SWITCH_SWITCH)
#define ADDRESS_SWITCH() MODULE_SWITCHING_GENERIC(ADDRESS_SWITCH_CHECK, ADDRESS_SWITCH_SWTICH)
#define NOTIFICATION_SWITCH() MODULE_SWITCHING_GENERIC(NOTIFICATION_SWITCH_CHECK, NOTIFICATION_SWITCH_SWTICH)

namespace Zscript {
namespace GenericCore {

template<class ZP>
class ModuleRegistry {
    static const uint16_t MAX_SYSTEM_CMD = 0xF;

public:
    static void execute(ZscriptCommandContext<ZP> ctx, bool moveAlong) {
        // commands are complete unless explicitly marked otherwise by a command
        ctx.commandComplete();
        OptInt16 value = ctx.getField(Zchars::Z_CMD);
        if (!value.isPresent) {
            ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
            return;
        }
        uint16_t cmd = value.value;
        if (cmd > MAX_SYSTEM_CMD && !ctx.isActivated()) {
            ctx.status(ResponseStatus::NOT_ACTIVATED);
            return;
        }
        uint8_t commandBottomBits = (uint8_t) (cmd & 0xF);
        (void) commandBottomBits;
        (void) moveAlong;

        switch (cmd >> 4) {
        MODULE_SWITCH()

    default:
        ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
        break;
        }

    }

#ifdef ZSCRIPT_SUPPORT_ADDRESSING
    static void execute(ZscriptAddressingContext<ZP> ctx, bool moveAlong) {
        ctx.commandComplete();
        if (!ctx.isActivated()) {
            ctx.status(ResponseStatus::NOT_ACTIVATED);
            return;
        }
        uint16_t addr = ctx.getAddressSegments().next().value;

        (void) moveAlong;
        (void) addr;

        switch (addr) {
        ADDRESS_SWITCH()

    default:
        ctx.status(ResponseStatus::ADDRESS_NOT_FOUND);
        break;
        }
    }
#endif

#ifdef ZSCRIPT_SUPPORT_NOTIFICATIONS
    static void notification(ZscriptNotificationContext<ZP> ctx, bool moveAlong) {
        uint8_t typeBits = ctx.getID() & 0xF;
        (void) typeBits;
        (void) moveAlong;
        switch (ctx.getID() >> 4) {
        NOTIFICATION_SWITCH()

    default:
        ctx.getOutStream().writeField(Zchars::Z_STATUS, ResponseStatus::INTERNAL_ERROR);
        }
    }
#endif
};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_MODULEREGISTRY_HPP_ */

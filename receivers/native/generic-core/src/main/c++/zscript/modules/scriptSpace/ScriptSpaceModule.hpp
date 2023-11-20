/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACEMODULE_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACEMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"

#define MODULE_EXISTS_002 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_002 MODULE_SWITCH_UTIL(ScriptSpaceModule<ZP>::execute)

#include "ScriptSpaceSetupCommand.hpp"
#include "ScriptSpaceWriteCommand.hpp"
#include "ScriptSpaceCapabilitiesCommand.hpp"

namespace Zscript {

namespace GenericCore {

template<class ZP>
class ScriptSpaceModule: public ZscriptModule<ZP> {
public:
    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {
        switch (bottomBits) {
        case ScriptSpaceCapabilitiesCommand<ZP>::CODE:
            ScriptSpaceCapabilitiesCommand<ZP>::execute(ctx);
            break;
        case ScriptSpaceSetupCommand<ZP>::CODE:
            ScriptSpaceSetupCommand<ZP>::execute(ctx);
            break;
        case ScriptSpaceWriteCommand<ZP>::CODE:
            ScriptSpaceWriteCommand<ZP>::execute(ctx);
            break;
        default:
            ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
            break;
        }
    }

};

}

}

#define ZSCRIPT_SUPPORT_SCRIPT_SPACE
#define ZSCRIPT_SUPPORT_NOTIFICATIONS

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACEMODULE_HPP_ */

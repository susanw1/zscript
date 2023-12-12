/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_OUTERCORE_OUTERCOREMODULE_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_OUTERCORE_OUTERCOREMODULE_HPP_

#ifdef ZSCRIPT_HPP_INCLUDED
#error Must be included before Zscript.hpp
#endif

#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"

#define MODULE_EXISTS_001 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_001 MODULE_SWITCH_UTIL(ZscriptOuterCoreModule<ZP>::execute)

#include "ExtendedCapabilitiesCommand.hpp"
#include "ChannelSetupCommand.hpp"

namespace Zscript {

namespace GenericCore {

template<class ZP>
class ZscriptOuterCoreModule: public ZscriptModule<ZP> {
public:

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {

        switch (bottomBits) {
        case ExtendedCapabilitiesCommand<ZP>::CODE:
            ExtendedCapabilitiesCommand<ZP>::execute(ctx, MODULE_CAPABILITIES(001));
            break;
#ifdef ZSCRIPT_RESET_COMMAND
        case outer_core_module::Reset_CommandDefs::CODE:
            ZP::ResetCommand::execute(ctx);
            break;
#endif
#ifdef ZSCRIPT_ID_SAVE_COMMAND
        case outer_core_module::WriteGuid_CommandDefs::CODE:
            ZP::IdCommand::saveId(ctx);
            break;
#endif
        case ChannelSetupCommand<ZP>::CODE:
            ChannelSetupCommand<ZP>::execute(ctx);
            break;
#ifdef ZSCRIPT_USER_CUSTOM_OUTER_CORE_COMMAND
        case outer_core_module::UserDefined_CommandDefs::CODE:
            ZP::CustomOuterCoreCommand::execute(ctx);
            break;
#endif
        default:
            ctx.status(ResponseStatus::COMMAND_NOT_FOUND);
            break;
        }
    }
};

}

}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_OUTERCORE_OUTERCOREMODULE_HPP_ */

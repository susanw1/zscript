/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_COREMODULE_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_COREMODULE_HPP_

#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"

#include "ActivateCommand.hpp"
#include "EchoCommand.hpp"
#include "MakeMatchCodeCommand.hpp"
#include "ChannelInfoCommand.hpp"

#define MODULE_EXISTS_000 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_000 MODULE_SWITCH_UTIL(ZscriptCoreModule<ZP>::execute)

#include "CapabilitiesCommand.hpp"

namespace Zscript {
namespace GenericCore {
template<class ZP>
class ZscriptCoreModule: public ZscriptModule<ZP> {

public:

    static void execute(ZscriptCommandContext<ZP> ctx, uint8_t bottomBits) {

        switch (bottomBits) {
        case 0x0:
            CapabilitiesCommand<ZP>::execute(ctx);
            break;
        case 0x1:
            EchoCommand<ZP>::execute(ctx);
            break;
        case 0x2:
            ActivateCommand<ZP>::execute(ctx);
            break;
#ifdef GUID_FETCH_COMMAND
        case 0x4:
            ZP::GuidCommand::fetchGuid(ctx);
            break;
#endif
        case 0x8:
            ChannelInfoCommand<ZP>::execute(ctx);
            break;
        case 0xc:
            MakeMatchCodeCommand<ZP>::makeCode(ctx);
            break;
        case 0xd:
            MakeMatchCodeCommand<ZP>::matchCode(ctx);
            break;
#ifdef USER_CUSTOM_CORE_COMMAND
        case 0x4:
            ZP::CustomCoreCommand::execute(ctx);
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

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_COREMODULE_HPP_ */

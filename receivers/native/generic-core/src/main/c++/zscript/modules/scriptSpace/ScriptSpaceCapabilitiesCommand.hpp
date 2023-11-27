/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACECAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACECAPABILITIESCOMMAND_HPP_

#include "../../ZscriptIncludes.hpp"

#include <net/zscript/model/modules/base/ScriptSpaceModule.hpp>
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"

#define COMMAND_EXISTS_0020 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace GenericCore {

template<class ZP>
class ScriptSpaceCapabilitiesCommand: public script_space_module::Capabilities_CommandDefs {
public:

    static void execute(ZscriptCommandContext<ZP> ctx, uint16_t commandsSet) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField(RespCommandsSet__C, commandsSet);
        out.writeField(RespScriptSpaceCount__P, Zscript<ZP>::zscript.getScriptSpaceCount());
    }
};

}

}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACECAPABILITIESCOMMAND_HPP_ */

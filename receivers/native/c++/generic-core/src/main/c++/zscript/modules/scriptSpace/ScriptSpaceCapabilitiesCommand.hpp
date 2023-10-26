/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACECAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACECAPABILITIESCOMMAND_HPP_
#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"
#define COMMAND_EXISTS_0020 EXISTENCE_MARKER_UTIL

namespace Zscript {
namespace GenericCore {
template<class ZP>
class ScriptSpaceCapabilitiesCommand {
public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        out.writeField('C', MODULE_CAPABILITIES(002));
        out.writeField('P', Zscript<ZP>::zscript.getScriptSpaceCount());

    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACECAPABILITIESCOMMAND_HPP_ */

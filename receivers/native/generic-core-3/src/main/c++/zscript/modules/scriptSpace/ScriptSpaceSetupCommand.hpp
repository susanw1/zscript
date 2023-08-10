/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACESETUPCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACESETUPCOMMAND_HPP_
#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"
#define COMMAND_EXISTS_0021 EXISTENCE_MARKER_UTIL

namespace Zscript {
template<class ZP>
class ScriptSpace;

namespace GenericCore {
template<class ZP>
class ScriptSpaceSetupCommand {
public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t spaceIndex = 0;
        if (!ctx.getField('P', &spaceIndex)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        } else if (spaceIndex >= ctx.getZscript()->getScriptSpaceCount()) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }
        CommandOutStream<ZP> out = ctx.getOutStream();
        ScriptSpace<ZP> *target = ctx.getZscript()->getScriptSpaces()[spaceIndex];
        out.writeField('P', target->getCurrentLength());
        if (target->isRunning()) {
            out.writeField('R', 0);
        }
        if (target->canBeWrittenTo()) {
            out.writeField('W', 0);
        }
        out.writeField('L', target->getMaxLength());
        uint16_t runOpt = 0;
        if (ctx.getField('R', &runOpt)) {
            if (runOpt != 0) {
                target->run();
            } else {
                target->stop();
            }
        }

    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACESETUPCOMMAND_HPP_ */

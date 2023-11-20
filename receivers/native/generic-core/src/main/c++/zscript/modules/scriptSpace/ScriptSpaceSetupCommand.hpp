/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACESETUPCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACESETUPCOMMAND_HPP_

#include "../../ZscriptIncludes.hpp"

#include <net/zscript/model/modules/base/ScriptSpaceModule.hpp>
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"

#define COMMAND_EXISTS_0021 EXISTENCE_MARKER_UTIL

namespace Zscript {

template<class ZP>
class ScriptSpace;

namespace GenericCore {

template<class ZP>
class ScriptSpaceSetupCommand: public script_space_module::ScriptSpaceSetup_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t spaceIndex = 0;
        if (!ctx.getReqdFieldCheckLimit(ReqScriptSpaceId__P, Zscript<ZP>::zscript.getScriptSpaceCount(), &spaceIndex)) {
            return;
        }

        CommandOutStream<ZP> out = ctx.getOutStream();
        ScriptSpace<ZP> *target = Zscript<ZP>::zscript.getScriptSpaces()[spaceIndex];
        out.writeField(RespCurrentWritePosition__P, target->getCurrentLength());
        if (target->isRunning()) {
            out.writeFlagFieldSet(RespRunning__R);
        }
        if (target->canBeWrittenTo()) {
            out.writeFlagFieldSet(RespWriteAllowed__W);
        }
        out.writeField(RespAvailableLength__L, target->getMaxLength());
        uint16_t runOpt = 0;
        if (ctx.getField(ReqRun__R, &runOpt)) {
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

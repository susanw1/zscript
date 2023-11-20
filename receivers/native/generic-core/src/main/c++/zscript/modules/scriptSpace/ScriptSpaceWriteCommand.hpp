/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACEWRITECOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACEWRITECOMMAND_HPP_

#include "../../ZscriptIncludes.hpp"

#include <net/zscript/model/modules/base/ScriptSpaceModule.hpp>
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"
#include "../../tokenizer/ZscriptTokenizer.hpp"

#define COMMAND_EXISTS_0022 EXISTENCE_MARKER_UTIL

namespace Zscript {

template<class ZP>
class ScriptSpace;

namespace GenericCore {

template<class ZP>
class ScriptSpaceWriteCommand: public script_space_module::ScriptSpaceWrite_CommandDefs {
private:
    static void writeToWriter(ZscriptCommandContext<ZP> ctx, ScriptSpace<ZP> *target, TokenRingBuffer<ZP> writer) {
        ZscriptTokenizer<ZP> tok(writer.getWriter(), 2);

        for (BigFieldBlockIterator<ZP> iter = ctx.getBigField(); iter.hasNext();) {
            DataArrayWLeng16 data = iter.nextContiguous();
            for (uint16_t i = 0; i < data.length; ++i) {
                tok.accept(data.data[i]);
            }
        }
        target->commitChanges(&writer);
    }

public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        uint16_t spaceIndex;
        if (!ctx.getField(ReqScriptSpaceId__P, &spaceIndex)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        } else if (spaceIndex >= Zscript<ZP>::zscript.getScriptSpaceCount()) {
            ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
            return;
        }

        CommandOutStream<ZP> out = ctx.getOutStream();
        ScriptSpace<ZP> *target = Zscript<ZP>::zscript.getScriptSpaces()[spaceIndex];
        if (target->isRunning()) {
            ctx.status(ResponseStatus::COMMAND_FAIL);
            return;
        }
        if (ctx.hasField(ReqReset__R)) {
            writeToWriter(ctx, target, target->overwrite());
        } else {
            writeToWriter(ctx, target, target->append());
        }
        out.writeField(RespAvailableLength__L, target->getRemainingLength());
    }
};

}

}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_SCRIPTSPACE_SCRIPTSPACEWRITECOMMAND_HPP_ */

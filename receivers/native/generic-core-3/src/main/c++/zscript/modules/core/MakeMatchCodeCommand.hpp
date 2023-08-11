/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_MAKEMATCHCODECOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_MAKEMATCHCODECOMMAND_HPP_

#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../../LanguageVersion.hpp"

#define COMMAND_EXISTS_000C EXISTENCE_MARKER_UTIL
#define COMMAND_EXISTS_000D EXISTENCE_MARKER_UTIL

namespace Zscript {
namespace GenericCore {
template<class ZP>
class MakeMatchCodeCommand {
    static uint16_t code;

public:
    static void makeCode(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        code = ZP::generateRandom16();
        out.writeField('C', code);
    }

    static void matchCode(ZscriptCommandContext<ZP> ctx) {
        uint16_t givenCode;
        if (!ctx.getField('C', &givenCode)) {
            ctx.status(ResponseStatus::MISSING_KEY);
            return;
        }
        if (givenCode != code) {
            ctx.status(ResponseStatus::COMMAND_FAIL);
        }
    }
};

template<class ZP>
uint16_t MakeMatchCodeCommand<ZP>::code = 0;
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_MAKEMATCHCODECOMMAND_HPP_ */

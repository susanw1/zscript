/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(SRC_ZSCRIPT_BASE_RESET_COMMAND)
#error This file should not be included more than once
#endif
#define SRC_ZSCRIPT_BASE_RESET_COMMAND

#include <zscript/modules/ZscriptCommand.hpp>
#include <net/zscript/model/modules/base/OuterCoreModule.hpp>

#define COMMAND_EXISTS_0011 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace outer_core_module {

template<class ZP>
class ZscriptResetCommand: public Reset_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        void (*resetFunc)(void) = 0;
        resetFunc();
    }
};

}

}

#define ZSCRIPT_RESET_COMMAND


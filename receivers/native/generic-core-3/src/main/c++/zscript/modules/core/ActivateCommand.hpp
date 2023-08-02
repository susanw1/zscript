/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_CORE_ACTIVATECOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_CORE_ACTIVATECOMMAND_HPP_
#include "../../ZscriptIncludes.hpp"
#include "../../execution/ZscriptCommandContext.hpp"
#include "../../LanguageVersion.hpp"

#define COMMAND_EXISTS_0002 EXISTENCE_MARKER_UTIL

namespace Zscript {
namespace GenericCore {
template<class ZP>
class ActivateCommand {
public:

    static void execute(ZscriptCommandContext<ZP> ctx) {
        AbstractOutStream<ZP> *out = ctx.getOutStream();
        out->writeField('A', ctx.isActivated());
        ctx.activate();
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_CORE_ACTIVATECOMMAND_HPP_ */

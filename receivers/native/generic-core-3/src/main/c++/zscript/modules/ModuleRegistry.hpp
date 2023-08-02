/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_MODULEREGISTRY_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_MODULEREGISTRY_HPP_
#include "../ZscriptIncludes.hpp"
#include "../execution/ZscriptAddressingContext.hpp"
#include "../execution/ZscriptCommandContext.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class ModuleRegistry {
public:
    static void execute(ZscriptCommandContext<ZP> cmdCtx) {
        //TODO
    }
    static void moveAlong(ZscriptCommandContext<ZP> cmdCtx) {
        //TODO
    }
    static void execute(ZscriptAddressingContext<ZP> addrCtx) {
        //TODO
    }
    static void moveAlong(ZscriptAddressingContext<ZP> addrCtx) {
        //TODO
    }
    static bool notification(AbstractOutStream<ZP> *out, int type, bool isAddressed) {
        //TODO
    }
};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_MODULEREGISTRY_HPP_ */

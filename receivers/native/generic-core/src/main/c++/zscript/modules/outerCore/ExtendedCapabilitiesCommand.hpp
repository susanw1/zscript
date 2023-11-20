/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_MODULES_OUTERCORE_EXTENDEDCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___ZSCRIPT_MODULES_OUTERCORE_EXTENDEDCAPABILITIESCOMMAND_HPP_

#include "../../ZscriptIncludes.hpp"

#include <net/zscript/model/modules/base/OuterCoreModule.hpp>
#include "../../execution/ZscriptCommandContext.hpp"
#include "../ZscriptModule.hpp"

#define COMMAND_EXISTS_0010 EXISTENCE_MARKER_UTIL

namespace Zscript {

namespace GenericCore {

template<class ZP>
class ExtendedCapabilitiesCommand: public outer_core_module::ExtendedCapabilities_CommandDefs {
public:
    static void execute(ZscriptCommandContext<ZP> ctx) {
        CommandOutStream<ZP> out = ctx.getOutStream();
        uint16_t target;
        if (ctx.getField(ReqModuleBankRequest__M, &target)) {
            if (target >= 16) {
                ctx.status(ResponseStatus::VALUE_OUT_OF_RANGE);
                return;
            }
            uint16_t result = 0;
            switch (target) {
            case 0:
                result = COMMAND_SWITCH_BYTE(0);
                break;
            case 1:
                result = COMMAND_SWITCH_BYTE(1);
                break;
            case 2:
                result = COMMAND_SWITCH_BYTE(2);
                break;
            case 3:
                result = COMMAND_SWITCH_BYTE(3);
                break;
            case 4:
                result = COMMAND_SWITCH_BYTE(4);
                break;
            case 5:
                result = COMMAND_SWITCH_BYTE(5);
                break;
            case 6:
                result = COMMAND_SWITCH_BYTE(6);
                break;
            case 7:
                result = COMMAND_SWITCH_BYTE(7);
                break;
            case 8:
                result = COMMAND_SWITCH_BYTE(8);
                break;
            case 9:
                result = COMMAND_SWITCH_BYTE(9);
                break;
            case 10:
                result = COMMAND_SWITCH_BYTE(a);
                break;
            case 11:
                result = COMMAND_SWITCH_BYTE(b);
                break;
            case 12:
                result = COMMAND_SWITCH_BYTE(c);
                break;
            case 13:
                result = COMMAND_SWITCH_BYTE(d);
                break;
            case 14:
                result = COMMAND_SWITCH_BYTE(e);
                break;
            case 15:
                result = COMMAND_SWITCH_BYTE(f);
                break;
            default:
                break;
            }
            out.writeField(RespModuleBanksLower__L, result);
        }
        out.writeField(RespCommandsSet__C, MODULE_CAPABILITIES(001));
        out.writeField(RespModuleBanksUpper__M, BROAD_MODULE_EXISTENCE);
    }
};

}

}

#endif /* SRC_MAIN_C___ZSCRIPT_MODULES_OUTERCORE_EXTENDEDCAPABILITIESCOMMAND_HPP_ */

/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTMODULE_HPP_
#define SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTMODULE_HPP_

#ifdef ZCODE_HPP_INCLUDED
#error Must be included before Zcode.hpp
#endif

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeModule.hpp"

#if defined(ZCODE_GENERATE_NOTIFICATIONS) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
#include "ZcodeInterruptSetupCommand.hpp"
#include "ZcodeInterruptSetCommand.hpp"
#endif
#include "ZcodeScriptStoreCommand.hpp"
#include "ZcodeScriptCapabilitiesCommand.hpp"

#ifndef ZCODE_SUPPORT_SCRIPT_SPACE
#error cannot use script space module without script space
#endif

#define MODULE_EXISTS_002 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_002 MODULE_SWITCH_UTIL( ZcodeScriptModule<ZP>::execute)

template<class ZP>
class ZcodeScriptModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {

        switch (bottomBits) {
        case 0x0:
            ZcodeScriptCapabilitiesCommand<ZP>::execute(slot);
            break;
        case 0x1:
            ZcodeScriptStoreCommand<ZP>::execute(slot);
            break;
#if defined(ZCODE_GENERATE_NOTIFICATIONS) && defined(ZCODE_SUPPORT_INTERRUPT_VECTOR)
        case 0x8:
            ZcodeInterruptSetupCommand<ZP>::execute(slot);
            break;
        case 0x9:
            ZcodeInterruptSetCommand<ZP>::execute(slot);
            break;
#endif
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }

};
// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_MAIN_CPP_ZCODE_MODULES_SCRIPT_ZCODESCRIPTMODULE_HPP_ */

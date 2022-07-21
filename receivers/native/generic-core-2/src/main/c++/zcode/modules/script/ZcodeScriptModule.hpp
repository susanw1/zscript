/*
 * ZcodeCoreModule.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTMODULE_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTMODULE_HPP_
#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error Must be included before Zcode.hpp
#endif

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeModule.hpp"
#include "ZcodeInterruptSetupCommand.hpp"
#include "ZcodeInterruptSetCommand.hpp"
#include "ZcodeScriptStoreCommand.hpp"
#include "ZcodeScriptCapabilitiesCommand.hpp"

#ifndef ZCODE_SUPPORT_SCRIPT_SPACE
#error cannot use script space module without script space
#endif

#define ZCODE_SCRIPT_MODULE_ADDRESS 0x02

#define MODULE_ADDRESS002 MODULE_ADDRESS_UTIL

#define COMMAND_SWITCH002 COMMAND_SWITCH_UTIL(ZCODE_SCRIPT_MODULE_ADDRESS, ZcodeScriptModule<ZP>::execute)

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

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODESCRIPTMODULE_HPP_ */

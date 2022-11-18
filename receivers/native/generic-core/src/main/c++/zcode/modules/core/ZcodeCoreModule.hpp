/*
 * ZcodeCoreModule.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_

#ifdef ZCODE_HPP_INCLUDED
#error Must be included before Zcode.hpp
#endif

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeModule.hpp"

#ifdef DEBUG_ADDRESSING
#include "ZcodeDebugAddressingSystem.hpp"
#endif

#include "ZcodeActivateCommand.hpp"
#include "ZcodeEchoCommand.hpp"
#include "ZcodeMakeCodeCommand.hpp"
#include "ZcodeMatchCodeCommand.hpp"
#include "ZcodeChannelInfoCommand.hpp"

#define MODULE_EXISTS_000 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_000 MODULE_SWITCH_UTIL(ZcodeCoreModule<ZP>::execute)

#include "ZcodeCapabilitiesCommand.hpp"

template<class ZP>
class ZcodeCoreModule: public ZcodeModule<ZP> {
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot, uint8_t bottomBits) {

        switch (bottomBits) {
        case 0x0:
            ZcodeCapabilitiesCommand<ZP>::execute(slot);
            break;
        case 0x1:
            ZcodeEchoCommand<ZP>::execute(slot);
            break;
        case 0x2:
            ZcodeActivateCommand<ZP>::execute(slot);
            break;
#ifdef ZCODE_CORE_RESET_COMMAND
        case 0x3:
            ZCODE_CORE_RESET_COMMAND(slot);
            break;
#endif
#ifdef ZCODE_CORE_FETCH_GUID
        case 0x4:
            ZCODE_CORE_FETCH_GUID(slot);
            break;
#endif
        case 0x8:
            ZcodeChannelInfoCommand<ZP>::execute(slot);
            break;
        case 0xc:
            ZcodeMakeCodeCommand<ZP>::execute(slot);
            break;
        case 0xd:
            ZcodeMatchCodeCommand<ZP>::execute(slot);
            break;
#ifdef ZCODE_CORE_USER_COMMAND
        case 0xf:
            ZCODE_CORE_USER_COMMAND(slot);
            break;
#endif
        default:
            slot.fail(UNKNOWN_CMD, (string_t) ZP::Strings::failParseUnknownCommand);
            break;
        }
    }

};
// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_ */

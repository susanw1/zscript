/*
 * ZcodeCoreModule.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_COMMANDS_ZCODECOREMODULE_H_
#define SRC_MAIN_C_ZCODE_COMMANDS_ZCODECOREMODULE_H_
#ifdef SRC_MAIN_C_ZCODE_ZCODECCODE_H_
#error Must be included before Zcode.h
#endif

#include "../../ZcodeIncludes.h"
#include "../ZcodeModule.h"

#include "ZcodeActivateCommand.h"
#include "ZcodeEchoCommand.h"
#include "ZcodeMakeCodeCommand.h"
#include "ZcodeMatchCodeCommand.h"
#include "ZcodeChannelInfoCommand.h"

#define MODULE_EXISTS_000 EXISTENCE_MARKER_UTIL
#define MODULE_SWITCH_000 ZcodeCoreModuleExecute(commandBottomBits);

#include "ZcodeCapabilitiesCommand.h"

void ZcodeCoreModuleExecute(uint8_t bottomBits) {

    switch (bottomBits) {
    case 0x0:
        ZcodeCapabilitiesCommandExecute();
        break;
    case 0x1:
        ZcodeEchoCommandExecute();
        break;
    case 0x2:
        ZcodeActivateCommandExecute();
        break;
#ifdef ZCODE_CORE_RESET_COMMAND
        case 0x3:
            ZCODE_CORE_RESET_COMMAND();
            break;
#endif
#ifdef ZCODE_CORE_FETCH_GUID
        case 0x4:
            ZCODE_CORE_FETCH_GUID();
            break;
#endif
    case 0x8:
        ZcodeChannelInfoCommandExecute();
        break;
    case 0xc:
        ZcodeMakeCodeCommandExecute();
        break;
    case 0xd:
        ZcodeMatchCodeCommandExecute();
        break;
#ifdef ZCODE_CORE_USER_COMMAND
        case 0xf:
            ZCODE_CORE_USER_COMMAND();
            break;
#endif
    default:
        ZcodeCommandFail(UNKNOWN_CMD);
        break;
    }
}

// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODECOREMODULE_HPP_ */

/*
 * ZcodeOuterCoreModule.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_MODULES_OUTER_CORE_ZCODEOUTERCOREMODULE_H_
#define SRC_MAIN_C_ZCODE_MODULES_OUTER_CORE_ZCODEOUTERCOREMODULE_H_
#ifdef SRC_MAIN_C_ZCODE_ZCODECCODE_H_
#error Must be included before Zcode.hpp
#endif

#include "../../ZcodeIncludes.h"
#include "../../Zcode.h"
#include "../ZcodeModule.h"
#include "ZcodeChannelSetupCommand.h"

#define MODULE_EXISTS_001 EXISTENCE_MARKER_UTIL

#include "ZcodeExtendedCapabilitiesCommand.h"

#define MODULE_SWITCH_001 ZcodeOuterCoreModuleExecute(commandBottomBits);

void ZcodeOuterCoreModuleExecute(uint8_t bottomBits) {

    switch (bottomBits) {
    case 0x0:
        ZcodeExtendedCapabilitiesCommandExecute();
        break;

#ifdef ZCODE_CORE_WRITE_GUID
        case 0x4:
        ZCODE_CORE_WRITE_GUID();
        break;
#endif
    case 0x8:
        ZcodeChannelSetupCommandExecute();
        break;
#ifdef ZCODE_OUTER_CORE_USER_COMMAND
        case 0xf:
        ZCODE_OUTER_CORE_USER_COMMAND();
        break;
#endif
    default:
        ZcodeCommandFail(UNKNOWN_CMD);
        break;
    }
}
// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_MAIN_C_ZCODE_MODULES_OUTER_CORE_ZCODEOUTERCOREMODULE_H_ */

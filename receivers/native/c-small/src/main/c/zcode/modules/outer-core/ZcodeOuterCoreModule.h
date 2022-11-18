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

#define MODULE_SWITCH_001 ZcodeOuterCoreModuleExecute(z, slot, commandBottomBits);

void ZcodeOuterCoreModuleExecute(Zcode *z, ZcodeCommandSlot *slot, uint8_t bottomBits) {

    switch (bottomBits) {
    case 0x0:
        ZcodeExtendedCapabilitiesCommandExecute(slot);
        break;

#ifdef ZCODE_CORE_WRITE_GUID
        case 0x4:
        ZCODE_CORE_WRITE_GUID(slot);
        break;
#endif
    case 0x8:
        ZcodeChannelSetupCommandExecute(slot);
        break;
#ifdef ZCODE_OUTER_CORE_USER_COMMAND
        case 0xf:
        ZCODE_OUTER_CORE_USER_COMMAND(slot);
        break;
#endif
    default:
        ZcodeCommandFail(slot, UNKNOWN_CMD);
        break;
    }
}
// want function of form: void execute(ZcodeRunningCommandSlot<ZP> *slot, uint16_t command)

#endif /* SRC_MAIN_C_ZCODE_MODULES_OUTER_CORE_ZCODEOUTERCOREMODULE_H_ */

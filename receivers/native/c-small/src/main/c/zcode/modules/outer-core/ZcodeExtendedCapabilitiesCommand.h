/*
 * ZcodeExtendedCapabilitiesCommand.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_H_
#define SRC_MAIN_C_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_H_

#include "../../ZcodeIncludes.h"
#include "../ZcodeCommand.h"
#include "../ZcodeModule.h"

#define COMMAND_VALUE_0010 MODULE_CAPABILITIES_UTIL

static void ZcodeExtendedCapabilitiesCommandExecute(ZcodeCommandSlot *slot) {
    uint16_t target;
    if (ZcodeFieldMapGet(slot->fieldMap, 'M', &target)) {
        if (target >= 16) {
            ZcodeCommandFail(slot, BAD_PARAM);
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
            result = COMMAND_SWITCH_BYTE(A);
            break;
        case 11:
            result = COMMAND_SWITCH_BYTE(B);
            break;
        case 12:
            result = COMMAND_SWITCH_BYTE(C);
            break;
        case 13:
            result = COMMAND_SWITCH_BYTE(D);
            break;
        case 14:
            result = COMMAND_SWITCH_BYTE(E);
            break;
        case 15:
            result = COMMAND_SWITCH_BYTE(F);
            break;
        default:
            break;
        }
        ZcodeOutStream_WriteField16('L', result);
    }
    ZcodeOutStream_WriteField16('C', MODULE_CAPABILITIES(001));
    ZcodeOutStream_WriteField16('M', BROAD_MODULE_EXISTENCE);

    ZcodeOutStream_WriteStatus(OK);
}

#endif /* SRC_MAIN_C_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_H_ */

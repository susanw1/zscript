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

static void ZcodeExtendedCapabilitiesCommandExecute() {
    uint16_t target;
    if (ZcodeFieldMapGetDest('M', &target)) {
        if (target >= 16) {
            ZcodeCommandFail(BAD_PARAM);
            return;
        }
#if COMMAND_SWITCH_BYTE(0)>0xff || \
    COMMAND_SWITCH_BYTE(1)>0xff || \
    COMMAND_SWITCH_BYTE(2)>0xff || \
    COMMAND_SWITCH_BYTE(3)>0xff || \
    COMMAND_SWITCH_BYTE(4)>0xff || \
    COMMAND_SWITCH_BYTE(5)>0xff || \
    COMMAND_SWITCH_BYTE(6)>0xff || \
    COMMAND_SWITCH_BYTE(7)>0xff || \
    COMMAND_SWITCH_BYTE(8)>0xff || \
    COMMAND_SWITCH_BYTE(9)>0xff || \
    COMMAND_SWITCH_BYTE(A)>0xff || \
    COMMAND_SWITCH_BYTE(B)>0xff || \
    COMMAND_SWITCH_BYTE(C)>0xff || \
    COMMAND_SWITCH_BYTE(D)>0xff || \
    COMMAND_SWITCH_BYTE(E)>0xff || \
    COMMAND_SWITCH_BYTE(F)>0xff
#define ZcodeExtendedCapabilitiesCommandExecute_RESULT_16
        uint16_t result = 0;
#else
        uint8_t result = 0;
#endif
        switch (target) {
#if COMMAND_SWITCH_BYTE(0) !=0
        case 0:
            result = COMMAND_SWITCH_BYTE(0);
            break;
#endif
#if COMMAND_SWITCH_BYTE(1) !=0
        case 1:
            result = COMMAND_SWITCH_BYTE(1);
            break;
#endif
#if COMMAND_SWITCH_BYTE(2) !=0
        case 2:
            result = COMMAND_SWITCH_BYTE(2);
            break;
#endif
#if COMMAND_SWITCH_BYTE(3) !=0
        case 3:
            result = COMMAND_SWITCH_BYTE(3);
            break;
#endif
#if COMMAND_SWITCH_BYTE(4) !=0
        case 4:
            result = COMMAND_SWITCH_BYTE(4);
            break;
#endif
#if COMMAND_SWITCH_BYTE(5) !=0
        case 5:
            result = COMMAND_SWITCH_BYTE(5);
            break;
#endif
#if COMMAND_SWITCH_BYTE(6) !=0
        case 6:
            result = COMMAND_SWITCH_BYTE(6);
            break;
#endif
#if COMMAND_SWITCH_BYTE(7) !=0
        case 7:
            result = COMMAND_SWITCH_BYTE(7);
            break;
#endif
#if COMMAND_SWITCH_BYTE(8) !=0
        case 8:
            result = COMMAND_SWITCH_BYTE(8);
            break;
#endif
#if COMMAND_SWITCH_BYTE(9) !=0
        case 9:
            result = COMMAND_SWITCH_BYTE(9);
            break;
#endif
#if COMMAND_SWITCH_BYTE(A) !=0
        case 10:
            result = COMMAND_SWITCH_BYTE(A);
            break;
#endif
#if COMMAND_SWITCH_BYTE(B) !=0
        case 11:
            result = COMMAND_SWITCH_BYTE(B);
            break;
#endif
#if COMMAND_SWITCH_BYTE(C) !=0
        case 12:
            result = COMMAND_SWITCH_BYTE(C);
            break;
#endif
#if COMMAND_SWITCH_BYTE(D) !=0
        case 13:
            result = COMMAND_SWITCH_BYTE(D);
            break;
#endif
#if COMMAND_SWITCH_BYTE(E) !=0
        case 14:
            result = COMMAND_SWITCH_BYTE(E);
            break;
#endif
#if COMMAND_SWITCH_BYTE(F) !=0
        case 15:
            result = COMMAND_SWITCH_BYTE(F);
            break;
#endif
        default:
            break;
        }
#ifdef ZcodeExtendedCapabilitiesCommandExecute_RESULT_16
        ZcodeOutStream_WriteField16('L', result);
#else
        ZcodeOutStream_WriteField8('L', result);
#endif
    }
#if MODULE_CAPABILITIES(001) >0xff
    ZcodeOutStream_WriteField16('C', MODULE_CAPABILITIES(001));
#else
    ZcodeOutStream_WriteField8('C', MODULE_CAPABILITIES(001));
#endif
#if BROAD_MODULE_EXISTENCE >0xff
    ZcodeOutStream_WriteField16('M', BROAD_MODULE_EXISTENCE);
#else
    ZcodeOutStream_WriteField8('M', BROAD_MODULE_EXISTENCE);
#endif

    ZcodeOutStream_WriteStatus(OK);
}

#endif /* SRC_MAIN_C_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_H_ */

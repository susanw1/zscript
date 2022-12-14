/*
 * ZcodeCommandFinder.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODECOMMANDFINDERCCODE_H_
#define SRC_MAIN_C_ZCODE_ZCODECOMMANDFINDERCCODE_H_

#include "ZcodeIncludes.h"
#include "modules/ZcodeModule.h"
#include "ZcodeRunnerCCode.h"
#include "Zcode.h"

#define MODULE_SWITCH_CHECK(x, y) MODULE_EXISTS_##x##y
#define MODULE_SWITCH_SWITCH(x, y) MODULE_SWITCH_##x##y break;

#define MODULE_SWITCH() MODULE_SWITCHING_GENERIC(MODULE_SWITCH_CHECK, MODULE_SWITCH_SWITCH)

#define MAX_SYSTEM_CODE  15

void runZcodeCommand() {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (slot->state.hasFailed) {
        ZcodeRunnerPerformFail();
        return;
    }
    if (slot->fieldMap.fieldCount == 0 && slot->bigField.pos == 0) {
        ZcodeOutStream_SilentSucceed();
        return;
    }

    uint16_t echoValue;
    if (ZcodeFieldMapGetDest(ECHO_PARAM, &echoValue)) {
        ZcodeOutStream_WriteField16(ECHO_PARAM, echoValue);
    }

    uint16_t command;
    if (!ZcodeFieldMapGetDest(CMD_PARAM, &command)) {
        ZcodeCommandFail(UNKNOWN_CMD);
        return;
    }

    if (command > MAX_SYSTEM_CODE && !zcode.state.activated) {
        ZcodeCommandFail(NOT_ACTIVATED);
        return;
    }

    uint8_t commandBottomBits = (uint8_t) (command & 0xF);
    (void) commandBottomBits;

    switch (command >> 4) {
    MODULE_SWITCH()

default:
    ZcodeCommandFail(UNKNOWN_CMD);
    break;
    }
}

#endif /* SRC_MAIN_C_ZCODE_ZCODECOMMANDFINDERCCODE_H_ */

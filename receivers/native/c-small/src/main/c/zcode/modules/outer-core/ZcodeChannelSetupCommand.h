/*
 * ZcodeChannelSetupCommand.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODECHANNELSETUPCOMMAND_H_
#define SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODECHANNELSETUPCOMMAND_H_

#include "../../ZcodeIncludes.h"
#include "../ZcodeCommand.h"

#define COMMAND_EXISTS_0018 EXISTENCE_MARKER_UTIL

void ZcodeChannelSetupCommandExecute(ZcodeCommandSlot *slot) {
    uint16_t targetIndex = 0;
    if (ZcodeFieldMapGet(slot->fieldMap, 'C', &targetIndex)) {
        if (targetIndex >= 1) {
            ZcodeCommandFail(slot, BAD_PARAM);
            return;
        } else {
            target = zcode->getChannel((uint8_t) targetIndex);
        }
    }
#ifdef ZCODE_GENERATE_NOTIFICATIONS
    if (ZcodeFieldMapGet(slot->fieldMap, 'N')) {
        ZcodeChannelSetNotifications();
    }
#endif
    ZcodeChannelReadSetup(slot);
}

#endif /* SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODECHANNELSETUPCOMMAND_H_ */

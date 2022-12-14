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

void ZcodeChannelSetupCommandExecute() {
    uint16_t targetIndex = 0;
    if (ZcodeFieldMapGetDest('C', &targetIndex)) {
        if (targetIndex >= 1) {
            ZcodeCommandFail(BAD_PARAM);
            return;
        }
    }
#ifdef ZCODE_GENERATE_NOTIFICATIONS
    if (ZcodeFieldMapGetDest( 'N')) {
        ZcodeChannelSetNotifications();
    }
#endif
    ZcodeChannelReadSetup();
}

#endif /* SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODECHANNELSETUPCOMMAND_H_ */

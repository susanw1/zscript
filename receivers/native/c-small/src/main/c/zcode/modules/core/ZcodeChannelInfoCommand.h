/*
 * ZcodeChannelInfoCommand.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODECHANNELINFOCOMMAND_H_
#define SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODECHANNELINFOCOMMAND_H_

#include "../../ZcodeIncludes.h"
#include "../ZcodeCommand.h"

#define COMMAND_EXISTS_0008 EXISTENCE_MARKER_UTIL

void ZcodeChannelInfoCommandExecute(ZcodeCommandSlot *slot) {
    uint16_t targetIndex = 0;
    if (ZcodeFieldMapGet(slot->fieldMap, 'C', &targetIndex)) {
        if (targetIndex >= 1) {
            ZcodeCommandFail(slot, BAD_PARAM);
            return;
        }
    }
    ZcodeOutStream_WriteField8('C', 1);
    ZcodeOutStream_WriteField8('U', 0);
    ZcodeChannelWriteInfo(slot);
}

#endif /* SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODECHANNELINFOCOMMAND_HPP_ */

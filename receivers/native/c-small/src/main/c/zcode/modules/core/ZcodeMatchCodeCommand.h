/*
 * ZcodeMatchCodeCommand.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODEMATCHCODECOMMAND_H_
#define SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODEMATCHCODECOMMAND_H_

#include "../../ZcodeIncludes.h"
#include "../ZcodeCommand.h"
#include "ZcodeMakeCodeCommand.h"

#define COMMAND_EXISTS_000D EXISTENCE_MARKER_UTIL

void ZcodeMatchCodeCommandExecute() {
    uint16_t value = 0;
    if (ZcodeFieldMapGetDest('C', &value)) {
        if (value == codeNumberForZcodeMatching) {
            ZcodeOutStream_WriteStatus(OK);
        } else {
            ZcodeCommandFail(CMD_FAIL);
        }
    } else {
        ZcodeCommandFail(BAD_PARAM);
    }
}

#endif /* SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODEMATCHCODECOMMAND_H_ */

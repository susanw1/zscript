/*
 * ZcodeMakeCodeCommand.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODEMAKECODECOMMAND_H_
#define SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODEMAKECODECOMMAND_H_

#include "../../ZcodeIncludes.h"
#include "../ZcodeCommand.h"

#define COMMAND_EXISTS_000C EXISTENCE_MARKER_UTIL

static uint16_t codeNumberForZcodeMatching;

void ZcodeMakeCodeCommandExecute(ZcodeCommandSlot *slot) {
    codeNumberForZcodeMatching++;
    ZcodeOutStream_WriteField16('C', codeNumberForZcodeMatching);
    ZcodeOutStream_WriteStatus(OK);
}
uint16_t codeNumberForZcodeMatching = 0;

#endif /* SRC_MAIN_C_ZCODE_MODULES_CORE_ZCODEMAKECODECOMMAND_H_ */

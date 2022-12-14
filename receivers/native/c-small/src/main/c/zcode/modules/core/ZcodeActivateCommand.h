/*
 * ZcodeActivateCommand.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_H_
#define SRC_MAIN_C_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_H_

#include "../../ZcodeIncludes.h"
#include "../../Zcode.h"
#include "../ZcodeCommand.h"
#include "../../ZcodeCCodeInclude.h"

#define COMMAND_EXISTS_0002 EXISTENCE_MARKER_UTIL

void ZcodeActivateCommandExecute() {
    ZcodeOutStream_WriteField8('A', (uint8_t) zcode.state.activated);
    ZcodeOutStream_WriteStatus(OK);
    zcode.state.activated = true;
}

#endif /* SRC_MAIN_C_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_H_ */

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

#define COMMAND_EXISTS_0002 EXISTENCE_MARKER_UTIL

void ZcodeActivateCommandExecute(Zcode *z) {
    ZcodeOutStream_WriteField8('A', (uint8_t) z->activated);
    ZcodeOutStream_WriteStatus(OK);
    z->activated = true;
}

#endif /* SRC_MAIN_C_ZCODE_COMMANDS_ZCODEACTIVATECOMMAND_H_ */

/*
 * ZcodeCapabilitiesCommand.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_H_
#define SRC_MAIN_C_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_H_

#include "../../ZcodeIncludes.h"
#include "../ZcodeCommand.h"
#include "../ZcodeModule.h"

#define COMMAND_EXISTS_0000 EXISTENCE_MARKER_UTIL

void ZcodeCapabilitiesCommandExecute(ZcodeCommandSlot *slot) {
    ZcodeOutStream_WriteStatus(OK);
    ZcodeOutStream_WriteField16('C', MODULE_CAPABILITIES(000));
    ZcodeOutStream_WriteField16('M', COMMAND_SWITCH_EXISTS_BOTTOM_BYTE(00));

    // TODO to be completed!
}

#endif /* SRC_MAIN_C_ZCODE_COMMANDS_ZCODECAPABILITIESCOMMAND_H_ */

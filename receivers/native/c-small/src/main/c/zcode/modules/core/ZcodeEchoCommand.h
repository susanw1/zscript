/*
 * ZcodeEchoCommand.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_COMMANDS_ZCODEECHOCOMMAND_H_
#define SRC_MAIN_C_ZCODE_COMMANDS_ZCODEECHOCOMMAND_H_

#include "../../ZcodeIncludes.h"
#include "../ZcodeCommand.h"

#define COMMAND_EXISTS_0001 EXISTENCE_MARKER_UTIL

void ZcodeEchoCommandExecute() {
    uint16_t statusResult;
    if (!ZcodeFieldMapGetDest(STATUS_RESP_PARAM, &statusResult)) {
        ZcodeOutStream_WriteStatus(OK);
    } else {
        ZcodeCommandFail((ZcodeResponseStatus) statusResult);
    }

    ZcodeFieldMapCopyToOutput();
    ZcodeBigFieldCopyToOutput();
}

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEECHOCOMMAND_HPP_ */

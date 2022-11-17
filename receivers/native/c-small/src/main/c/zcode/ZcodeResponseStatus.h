/*
 * ZcodeResponseStatus.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODERESPONSESTATUS_H_
#define SRC_MAIN_C_ZCODE_ZCODERESPONSESTATUS_H_

#include "ZcodeIncludes.h"

enum ZcodeResponseStatus {
    OK = 0,
    RESP_TOO_LONG = 1,
    UNKNOWN_ERROR = 2,
    PARSE_ERROR = 3,
    UNKNOWN_CMD = 4,
    MISSING_PARAM = 5,
    BAD_PARAM = 6,
    NOT_ACTIVATED = 7,
    TOO_BIG = 8,
    BAD_ADDRESSING = 9,
    RETRY_REQEST = 0xa,
    CMD_ERROR = 0xf,
    CMD_FAIL = 0x10,

};
typedef enum ZcodeResponseStatus ZcodeResponseStatus;

#endif /* SRC_MAIN_C_ZCODE_ZCODERESPONSESTATUS_H_ */

/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_ZCODERESPONSESTATUS_HPP_
#define SRC_MAIN_CPP_ZCODE_ZCODERESPONSESTATUS_HPP_

#include "ZcodeIncludes.hpp"

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

#endif /* SRC_MAIN_CPP_ZCODE_ZCODERESPONSESTATUS_HPP_ */

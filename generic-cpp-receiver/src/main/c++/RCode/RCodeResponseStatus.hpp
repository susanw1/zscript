/*
 * RCodeResponseStatus.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODERESPONSESTATUS_HPP_
#define SRC_TEST_CPP_RCODE_RCODERESPONSESTATUS_HPP_
#include "RCodeIncludes.hpp"

enum RCodeResponseStatus {
    OK = 0,
    RESP_TOO_LONG = 1,
    UNKNOWN_ERROR = 2,
    PARSE_ERROR = 3,
    UNKNOWN_CMD = 4,
    MISSING_PARAM = 5,
    BAD_PARAM = 6,
    NOT_ACTIVATED = 7,
    TOO_BIG = 8,
    SETUP_ERROR = 0xf,

    CMD_FAIL = 0x10,
    NOT_EXECUTED = 0x11
};

#endif /* SRC_TEST_CPP_RCODE_RCODERESPONSESTATUS_HPP_ */

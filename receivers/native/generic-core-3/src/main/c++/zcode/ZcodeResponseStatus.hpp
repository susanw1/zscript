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
    /** command success */
    SUCCESS = 0x0,

    /** command failure */
    COMMAND_FAIL = 0x1,
    /** command failed in order to control the program flow */
    COMMAND_FAIL_CONTROL = 0x2,
    /** target of command either non-existent or not responding */
    COMMAND_NO_TARGET = 0x3,
    /** target of command does not support the requested functionality */
    COMMAND_TARGET_DOES_NOT_SUPPORT = 0x4,
    /** data transmission not successful - may have failed or only been partial */
    COMMAND_DATA_NOT_TRANSMITTED = 0x8,
    /** data reception not successful - may have failed or only been partial */
    COMMAND_DATA_NOT_RECEIVED = 0x9,

    /** buffer overflow */
    BUFFER_OVR_ERROR = 0x10,
    /** impossible condition hit */
    INTERNAL_ERROR = 0x11,
    /** channel not activated for non-core command */
    NOT_ACTIVATED = 0x12,
    /** command error in order to control the program flow */
    COMMAND_ERROR_CONTROL = 0x13,

    /** numeric field too long */
    FIELD_TOO_LONG = 0x20,
    /** numeric field too long */
    ODD_LENGTH = 0x21,
    /** missing end quote */
    UNTERMINATED_STRING = 0x22,
    /** string escaping invalid */
    ESCAPING_ERROR = 0x23,
    /** illegal character given as token key */
    ILLEGAL_KEY = 0x24,

    /** locks field too long or repeated */
    INVALID_LOCKS = 0x30,
    /** echo field too long or repeated */
    INVALID_ECHO = 0x31,
    /** comment with non-comment fields found */
    INVALID_COMMENT = 0x32,

    /** two fields (in the same command) with the same key found */
    REPEATED_KEY = 0x43,
    /** field not valid in this position */
    INVALID_KEY = 0x44,
    /** given command does not exist or not specified */
    COMMAND_NOT_FOUND = 0x45,
    /** given address does not exist or cannot be addressed */
    ADDRESS_NOT_FOUND = 0x46,

    /** a required field was not given */
    MISSING_KEY = 0x50,
    /** a field or big field was given with a value which is not supported */
    VALUE_OUT_OF_RANGE = 0x51,
    /** data transmission/reception exceeds maximum transmission length */
    DATA_TOO_LONG = 0x52,

    /** generic failure mid-execution */
    EXECUTION_ERROR = 0x80,

    /** a peripheral required for the command has failed fatally */
    PERIPHERAL_ERROR = 0x90,
    /** a peripheral required for the command is in a state it cannot exit (and may require rebooting) */
    PERIPHERAL_JAM = 0x91,
    /** a peripheral required for the command is not currently enabled (and may require other commands or activity to enable) */
    PERIPHERAL_DISABLED = 0x92,
    /** a peripheral required for the command is not attached (and may require physical intervention to attach) */
    PERIPHERAL_DISCONNECTED = 0x93,
    /** a peripheral required for the command has sustained permanent damage (and may require replacement) */
    PERIPHERAL_BROKEN = 0x94
};
bool zcodeResponseStatusIsSuccess(uint8_t code) {
    return code == SUCCESS;
}

bool zcodeResponseStatusIsError(uint8_t code) {
    return (code & ~0xf) != 0;
}

bool zcodeResponseStatusIsFailure(uint8_t code) {
    return (code & ~0xf) == 0 && code != SUCCESS;
}

#endif /* SRC_MAIN_CPP_ZCODE_ZCODERESPONSESTATUS_HPP_ */

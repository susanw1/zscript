/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

enum Zchars {
    CMD_PARAM = 'Z',
    ECHO_PARAM = 'E',
    STATUS_RESP_PARAM = 'S',
    NOTIFY_TYPE_PARAM = 'Z',

    ANDTHEN_SYMBOL = '&',
    ORELSE_SYMBOL = '|',
    EOL_SYMBOL = '\n',
    STRING_ESCAPE_SYMBOL = '=',

    BIGFIELD_PREFIX_MARKER = '+',
    BIGFIELD_QUOTE_MARKER = '"',

    BROADCAST_PREFIX = '*',
    DEBUG_PREFIX = '#',
    PARALLEL_PREFIX = '%',
    NOTIFY_PREFIX = '!',
    ADDRESS_PREFIX = '@',
    ADDRESS_SEPARATOR = '.'
};

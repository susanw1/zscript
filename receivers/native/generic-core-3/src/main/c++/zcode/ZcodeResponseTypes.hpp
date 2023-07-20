/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_ZCODERESPONSETYPES_HPP_
#define SRC_MAIN_CPP_ZCODE_ZCODERESPONSETYPES_HPP_

enum ZcodeResponseType {
    Response = 0,
    ResetNotification = 1,
    PeripheralNotification = 2,
    ScriptSpaceNotification = 3
};

#endif /* SRC_MAIN_CPP_ZCODE_ZCODERESPONSETYPES_HPP_ */

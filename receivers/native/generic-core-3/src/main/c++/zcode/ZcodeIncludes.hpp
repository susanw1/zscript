/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZCODE_ZCODEINCLUDES_HPP_
#define SRC_MAIN_C___ZCODE_ZCODEINCLUDES_HPP_

#include <stdint.h>
#include <stdlib.h>

#include "ZcodeLanguageVersion.hpp"

#include "ZcodeResponseStatus.hpp"
#include "Zchars.hpp"
#include "ZcodeResponseTypes.hpp"

struct DataArrayWLeng8 {
    uint8_t *data;
    uint8_t length;
};

struct DataArrayWLeng16 {
    uint8_t *data;
    uint16_t length;
};

struct DataArrayWLeng32 {
    uint8_t *data;
    uint32_t length;
};

#endif /* SRC_MAIN_C___ZCODE_ZCODEINCLUDES_HPP_ */

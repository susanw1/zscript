/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_ZSCRIPTINCLUDES_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZSCRIPTINCLUDES_HPP_

// Retained rather than cstdint etc (not supported on Arduino env)
#include <stdint.h>
#include <stdlib.h>

#include "LanguageVersion.hpp"
#include "Zchars.hpp"
#include "ZscriptResponseStatus.hpp"

namespace Zscript {

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

struct OptInt16 {
    uint16_t value;
    bool isPresent;

    OptInt16() : value(0), isPresent(false) {
    }

    OptInt16(uint16_t value, bool isPresent) : value(value), isPresent(isPresent){
    }

    static OptInt16 empty() {
        return {};
    }

    static OptInt16 of(uint16_t value) {
        return {value, true};
    }
};

}
#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPTINCLUDES_HPP_ */

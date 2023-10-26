/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_EXECUTION_DISCARDINGOUTSTREAM_HPP_
#define SRC_MAIN_C___ZSCRIPT_EXECUTION_DISCARDINGOUTSTREAM_HPP_
#include "../ZscriptIncludes.hpp"
#include "../AbstractOutStream.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class DiscardingOutStream: public AbstractOutStream<ZP> {
    bool openB = false;

public:
    void writeBytes(const uint8_t *bytes, uint16_t count, bool hexMode) {
        (void) bytes;
        (void) count;
        (void) hexMode;
    }

    void open(uint8_t sourceInd) {
        (void) sourceInd;
        openB = true;
    }
    void close() {
        openB = false;
    }
    bool isOpen() {
        return openB;
    }
};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_EXECUTION_DISCARDINGOUTSTREAM_HPP_ */

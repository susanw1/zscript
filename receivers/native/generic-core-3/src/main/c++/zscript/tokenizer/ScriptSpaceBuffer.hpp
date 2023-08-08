/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACEBUFFER_HPP_
#define SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACEBUFFER_HPP_
#include "../ZscriptIncludes.hpp"
#include "TokenRingBuffer.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class ScriptSpaceBuffer {
    TokenRingBuffer<ZP> readBuffer;
    bool canBeWrittenB;
    uint16_t maxLength;

public:
    ScriptSpaceBuffer(uint8_t *buffer, uint16_t maxLength) :
            readBuffer(buffer, 0), canBeWrittenB(true), maxLength(maxLength) {
        readBuffer.disableWriteStart();
    }

    ScriptSpaceBuffer(uint8_t *buffer, uint16_t maxLength, uint16_t initialLength, bool canBeWritten) :
            readBuffer(buffer, initialLength), canBeWrittenB(canBeWritten), maxLength(maxLength) {
        readBuffer.disableWriteStart();
    }

    TokenRingBuffer<ZP>* getReadBuffer() {
        return &readBuffer;
    }

    TokenRingBuffer<ZP> fromStart() {
        return TokenRingBuffer<ZP>(readBuffer.getData(), maxLength, 0);
    }
    TokenRingBuffer<ZP> append() {
        return TokenRingBuffer<ZP>(readBuffer.getData(), maxLength, readBuffer.getLength());
    }

    void commitChanges(TokenRingBuffer<ZP> *newSource) {
        readBuffer = TokenRingBuffer<ZP>(newSource->getData(), newSource->getWritePos());
        readBuffer.disableWriteStart();
    }

    bool canBeWritten() {
        return canBeWrittenB;
    }

    uint16_t getMaxLength() {
        return maxLength;
    }

    uint16_t getRemainingLength() {
        return maxLength - readBuffer.getLength();
    }

};

}
}

#endif /* SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACEBUFFER_HPP_ */

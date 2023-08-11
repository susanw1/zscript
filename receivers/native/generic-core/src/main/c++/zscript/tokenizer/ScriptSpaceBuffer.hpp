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
    typedef typename ZP::tokenBufferSize_t tokenBufferSize_t;
    TokenRingBuffer<ZP> readBuffer;
    bool canBeWrittenB;
    tokenBufferSize_t maxLength;

public:
    ScriptSpaceBuffer(uint8_t *buffer, tokenBufferSize_t maxLength) :
            readBuffer(buffer, 0), canBeWrittenB(true), maxLength(maxLength) {
        readBuffer.disableWriteStart();
    }

    ScriptSpaceBuffer(uint8_t *buffer, tokenBufferSize_t maxLength, tokenBufferSize_t initialLength, bool canBeWritten) :
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

    tokenBufferSize_t getMaxLength() {
        return maxLength;
    }

    tokenBufferSize_t getRemainingLength() {
        return maxLength - readBuffer.getLength();
    }

};

}
}

#endif /* SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACEBUFFER_HPP_ */

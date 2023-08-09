/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACE_HPP_
#define SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACE_HPP_
#include "../ZscriptIncludes.hpp"
#include "../tokenizer/ScriptSpaceBuffer.hpp"
#include "../ZscriptChannel.hpp"
#include "ScriptSpaceOutStream.hpp"

namespace Zscript {
template<class ZP>
class ScriptSpace: public ZscriptChannel<ZP> {
    GenericCore::ScriptSpaceBuffer<ZP> buffer;
    GenericCore::ScriptSpaceOutStream<ZP> out;
    bool canBeWrittenToB;

public:
    ScriptSpace(Zscript<ZP> *z, uint8_t *scriptSpace, uint16_t spaceLength, uint8_t *outBuffer, uint16_t outBufferSize) :
            ZscriptChannel<ZP>(buffer.getReadBuffer(), &out), buffer(scriptSpace, spaceLength), out(z, &this->parser, outBuffer, outBufferSize), canBeWrittenToB(true) {
        stop();
    }
    ScriptSpace(Zscript<ZP> *z, uint8_t *scriptSpace, uint16_t spaceLength, uint16_t initialDataLength, bool canWrite, uint8_t *outBuffer, uint16_t outBufferSize) :
            ZscriptChannel<ZP>(buffer.getReadBuffer(), &out), buffer(scriptSpace, spaceLength, initialDataLength, canWrite), out(z, &this->parser, outBuffer, outBufferSize), canBeWrittenToB(
                    canWrite) {
    }
    void channelInfo(ZscriptCommandContext<ZP> ctx) {

    }

    void channelSetup(ZscriptCommandContext<ZP> ctx) {

    }

    bool canBeWrittenTo() {
        return canBeWrittenToB;
    }

    GenericCore::TokenRingBuffer<ZP> append() {
        if (canBeWrittenToB) {
            return buffer.append();
        } else {
            //unreachable
        }
    }

    GenericCore::TokenRingBuffer<ZP> overwrite() {
        if (canBeWrittenToB) {
            return buffer.fromStart();
        } else {
            //unreachable
        }
    }

    void commitChanges(GenericCore::TokenRingBuffer<ZP> *newSource) {
        buffer.commitChanges(newSource);
    }
    uint16_t getCurrentLength() {
        return this->buffer.getReadBuffer()->getLength();
    }
    uint16_t getRemainingLength() {
        return this->buffer.getRemainingLength();
    }
    uint16_t getMaxLength() {
        return this->buffer.getMaxLength();
    }

    bool isRunning() {
        return this->parser.isRunning();
    }

    void run() {
        this->parser.resume();
    }

    void stop() {
        this->parser.stop();
    }

};
}

#endif /* SRC_MAIN_C___ZSCRIPT_SCRIPTSPACE_SCRIPTSPACE_HPP_ */

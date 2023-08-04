/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_ZSCRIPTCHANNEL_HPP_
#define SRC_MAIN_C___ZSCRIPT_ZSCRIPTCHANNEL_HPP_

#include "semanticParser/SemanticParser.hpp"
#include "execution/ZscriptCommandContext.hpp"
#include "AbstractOutStream.hpp"

namespace Zscript {

template<class ZP>
class ZscriptChannel {
    GenericCore::SemanticParser<ZP> parser;
    AbstractOutStream<ZP> *out;

public:
    ZscriptChannel(GenericCore::TokenRingBuffer<ZP> *buffer, AbstractOutStream<ZP> *out) :
            parser(buffer), out(out) {
    }

    GenericCore::SemanticParser<ZP>* getParser() {
        return &parser;
    }

    AbstractOutStream<ZP>* getOutStream() {
        return out;
    }

    void setChannelIndex(uint8_t i) {
        parser.setChannelIndex(i);
    }

    virtual void moveAlong() {
    }

    virtual void channelInfo(ZscriptCommandContext<ZP> ctx) {

    }

    virtual void channelSetup(ZscriptCommandContext<ZP> ctx) {

    }

};
}
#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPTCHANNEL_HPP_ */

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
protected:
    AbstractOutStream<ZP> *out;
    GenericCore::SemanticParser<ZP> parser;

public:
    ZscriptChannel(AbstractOutStream<ZP> *out, GenericCore::TokenRingBuffer<ZP> *buffer, bool canBeNotifChannel) :
            out(out), parser(buffer) {
        parser.freeBool = canBeNotifChannel;
    }

    bool canBeNotifChannel() {
        return parser.freeBool;
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
        (void) ctx;
    }

    virtual void channelSetup(ZscriptCommandContext<ZP> ctx) {
        (void) ctx;
    }

};
}
#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPTCHANNEL_HPP_ */

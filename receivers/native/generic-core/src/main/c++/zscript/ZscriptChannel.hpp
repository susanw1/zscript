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
    uint16_t moduleNumber;

public:
    ZscriptChannel(AbstractOutStream<ZP> *out, GenericCore::TokenRingBuffer<ZP> *buffer, uint16_t moduleNumber, bool canBeNotifChannel) :
            out(out), parser(buffer), moduleNumber(moduleNumber) {
        parser.freeBool = canBeNotifChannel;
    }

    bool canBeNotifChannel() {
        return parser.freeBool;
    }

    GenericCore::SemanticParser<ZP> *getParser() {
        return &parser;
    }

    AbstractOutStream<ZP> *getOutStream() {
        return out;
    }

    void setChannelIndex(uint8_t i) {
        parser.setChannelIndex(i);
    }

#ifdef ZSCRIPT_SUPPORT_PERSISTENCE
    virtual bool setupStartupNotificationChannel() {
        return false;
    }
#endif

    virtual void moveAlong() {
    }

    uint16_t getAssociatedModule() {
        return moduleNumber;
    }

    uint16_t getBufferLength() {
        return parser.getReader().asBuffer()->getLength();
    }

    void reset() {
        parser.deActivate();
    }

};
}
#endif /* SRC_MAIN_C___ZSCRIPT_ZSCRIPTCHANNEL_HPP_ */

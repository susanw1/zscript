/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_CHANNELS_ZCODECOMMANDCHANNEL_HPP_
#define SRC_MAIN_CPP_ZCODE_CHANNELS_ZCODECOMMANDCHANNEL_HPP_

#include "../ZcodeIncludes.hpp"
#include "../running/ZcodeRunningCommandSlot.hpp"

template<class ZP>
class ZcodeLockSet;

template<class ZP>
class ZcodeChannelInStream;

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeCommandSequence;

enum ZcodeCommandChannelStateChange {
    SET_AS_NOTIFICATION, RELEASED_FROM_NOTIFICATION, SET_AS_DEBUG, RELEASED_FROM_DEBUG
};
template<class ZP>
class ZcodeCommandChannel {
public:
    ZcodeChannelInStream<ZP> *in;
    ZcodeOutStream<ZP> *out;
    ZcodeRunningCommandSlot<ZP> runner;
    bool packetBased;

    ZcodeCommandChannel(ZcodeChannelInStream<ZP> *in, ZcodeOutStream<ZP> *out, bool packetBased) :
            in(in), out(out), runner(out, in->getSlot()), packetBased(packetBased) {
    }

    virtual void stateChange(ZcodeCommandChannelStateChange change) {
        (void) (change);
    }

    virtual void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) = 0;

    virtual void readSetup(ZcodeExecutionCommandSlot<ZP> slot) = 0;

    virtual bool isFromNotification() {
        return true;
    }

    virtual bool reset() {
        return false;
    }

};

#endif /* SRC_MAIN_CPP_ZCODE_CHANNELS_ZCODECOMMANDCHANNEL_HPP_ */
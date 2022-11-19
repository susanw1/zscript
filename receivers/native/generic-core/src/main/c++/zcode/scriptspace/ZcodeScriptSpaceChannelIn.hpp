/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODESCRIPTSPACECHANNELIN_HPP_
#define SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODESCRIPTSPACECHANNELIN_HPP_

#include "../channels/ZcodeChannelInStream.hpp"
#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeScriptSpace;
template<class ZP>
class ZcodeScriptSpaceChannel;
template<class ZP>
class Zcode;

template<class ZP>
class ZcodeScriptSpaceChannelIn: public ZcodeChannelInStream<ZP> {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;
    ZcodeScriptSpace<ZP> *space;
    ZcodeScriptSpaceChannel<ZP> *channel;
    scriptSpaceAddress_t pos = 0;
    uint8_t big[ZP::scriptBigSize];
    uint8_t delay = 0;

public:
    ZcodeScriptSpaceChannelIn(ZcodeScriptSpaceChannel<ZP> *channel) :
            ZcodeChannelInStream<ZP>(channel, big, ZP::scriptBigSize), space(Zcode<ZP>::zcode.getSpace()), channel(channel) {
    }

    void initialSetup(ZcodeScriptSpace<ZP> *space) {
        this->space = space;
    }

    bool pushData() {
        if (!space->isRunning()) {
            return false;
        }
        if (channel->getOutStream()->isDataBufferFull()) {
            channel->getOutStream()->flush();
            return false;
        } else {
            if (delay == 0) {
                char data = 0;
                if (pos >= space->getLength()) {
                    data = Zchars::EOL_SYMBOL;
                } else {
                    data = (char) space->get(pos++);
                }
                if (pos >= space->getLength()) {
                    delay = space->getDelay();
                    pos = 0;
                }
                return this->slot.acceptByte(data);
            } else {
                delay--;
                return false;
            }
        }

    }

    void setPosition(scriptSpaceAddress_t position) {
        pos = position;
    }

    scriptSpaceAddress_t getPosition() {
        return pos;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODESCRIPTSPACECHANNELIN_HPP_ */

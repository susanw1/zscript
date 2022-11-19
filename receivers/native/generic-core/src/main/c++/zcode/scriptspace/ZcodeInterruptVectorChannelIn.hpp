/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODEINTERRUPTVECTORCHANNELIN_HPP_
#define SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODEINTERRUPTVECTORCHANNELIN_HPP_

#include "../ZcodeIncludes.hpp"
#include "../channels/ZcodeChannelInStream.hpp"

template<class ZP>
class ZcodeScriptSpace;
template<class ZP>
class Zcode;

template<class ZP>
class ZcodeInterruptVectorChannelIn: public ZcodeChannelInStream<ZP> {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;
    ZcodeScriptSpace<ZP> *space;
    scriptSpaceAddress_t pos = 0;
    uint8_t big[ZP::scriptBigSize];
    bool running = false;

public:
    ZcodeInterruptVectorChannelIn(ZcodeCommandChannel<ZP> *channel) :
            ZcodeChannelInStream<ZP>(channel, big, ZP::scriptBigSize), space(Zcode<ZP>::zcode.getSpace()) {
    }

    void initialSetup(ZcodeScriptSpace<ZP> *space) {
        this->space = space;
    }
    void start(scriptSpaceAddress_t position) {
        pos = position;
        running = true;
    }

    bool pushData() {
        if (!running) {
            return false;
        }
        char data = 0;
        if (pos >= space->getLength() || data == Zchars::EOL_SYMBOL) {
            running = false;
            data = Zchars::EOL_SYMBOL;
        } else {
            data = (char) space->get(pos++);
        }
        return this->slot.acceptByte(data);
    }

    scriptSpaceAddress_t getPosition() {
        return pos;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODEINTERRUPTVECTORCHANNELIN_HPP_ */

/*
 * ZcodeExecutionSpaceSequenceIn.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNELIN_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNELIN_HPP_

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
    ZcodeInterruptVectorChannelIn(Zcode<ZP> *zcode, ZcodeCommandChannel<ZP> *channel) :
            ZcodeChannelInStream<ZP>(zcode, channel, big, ZP::scriptBigSize), space(zcode->getSpace()) {
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

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNELIN_HPP_ */

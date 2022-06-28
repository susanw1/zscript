/*
 * ZcodeExecutionSpaceSequenceIn.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACESEQUENCEIN_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACESEQUENCEIN_HPP_

#include "../channels/ZcodeChannelInStream.hpp"
#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeScriptSpace;
template<class ZP>
class Zcode;

template<class ZP>
class ZcodeScriptSpaceChannelIn: public ZcodeChannelInStream<ZP> {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;
    ZcodeScriptSpace<ZP> *space;
    scriptSpaceAddress_t pos = 0;
    uint8_t big[ZP::scriptBigSize];
    uint8_t delay = 0;

public:
    ZcodeScriptSpaceChannelIn(Zcode<ZP> *zcode, ZcodeCommandChannel<ZP> *channel) :
            ZcodeChannelInStream<ZP>(zcode, channel, big, ZP::scriptBigSize), space(zcode->getSpace()) {
    }

    void initialSetup(ZcodeScriptSpace<ZP> *space) {
        this->space = space;
    }

    bool pushData() {
        if (!space->isRunning()) {
            return false;
        }
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

    void setPosition(scriptSpaceAddress_t position) {
        pos = position;
    }

    scriptSpaceAddress_t getPosition() {
        return pos;
    }
};

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACESEQUENCEIN_HPP_ */

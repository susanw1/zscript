/*
 * ZcodeExecutionSpaceChannel.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_

#include "../channels/ZcodeCommandChannel.hpp"
#include "../scriptspace/ZcodeScriptSpaceChannelIn.hpp"
#include "../scriptspace/ZcodeScriptSpaceOut.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeScriptSpace;

template<class ZP>
class ZcodeScriptSpaceChannel: public ZcodeCommandChannel<ZP> {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;

    ZcodeScriptSpace<ZP> *space;
    ZcodeScriptSpaceOut<ZP> outA;
    ZcodeScriptSpaceChannelIn<ZP> inA;

public:

    ZcodeScriptSpaceChannel(Zcode<ZP> *zcode) :
            ZcodeCommandChannel<ZP>(zcode, &inA, &outA, false), space(zcode->getSpace()), outA(space), inA(zcode, this) {
    }

    ZcodeScriptSpaceOut<ZP>* getOutStream() {
        return &outA;
    }

    void move(scriptSpaceAddress_t position) {
        inA.setPosition(position);
    }

};

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_ */

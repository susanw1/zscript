/*
 * ZcodeInterruptVectorChannel.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORCHANNEL_HPP_
#define SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORCHANNEL_HPP_

#include "ZcodeInterruptVectorChannelIn.hpp"
#include "ZcodeInterruptVectorOut.hpp"
#include "../ZcodeIncludes.hpp"
#include "../ZcodeLockSet.hpp"
#include "../ZcodeBusInterrupt.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeInterruptVectorManager;

template<class ZP>
class ZcodeInterruptVectorChannel: public ZcodeCommandChannel<ZP> {
private:
    ZcodeBusInterrupt<ZP> interrupt;
    ZcodeInterruptVectorChannelIn<ZP> inA;
    ZcodeInterruptVectorOut<ZP> outA;

public:
    ZcodeInterruptVectorChannel() :
            ZcodeCommandChannel<ZP>(&inA, &outA, false), inA(this), outA() {
    }

    void setInterrupt(ZcodeBusInterrupt<ZP> interrupt) {
        this->interrupt = interrupt;
        inA.start(Zcode<ZP>::zcode.getNotificationManager()->getVectorChannel()->findVector(&interrupt));
    }
    void clear() {
        interrupt.clear();
    }

    ZcodeBusInterrupt<ZP>* getInterrupt() {
        return &interrupt;
    }
    bool isInUse() {
        return interrupt.getSource() != NULL || outA.isOpen();
    }

    void giveInfo(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField16('B', ZP::scriptBigSize);
        out->writeField16('F', ZP::fieldNum);
        out->writeField8('M', 2);
        out->writeBigStringField(ZP::Strings::interruptVectorChannelDescriptor);
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        //TODO: proper setup
    }
};
#include "../scriptspace/ZcodeInterruptVectorManager.hpp"

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORCHANNEL_HPP_ */

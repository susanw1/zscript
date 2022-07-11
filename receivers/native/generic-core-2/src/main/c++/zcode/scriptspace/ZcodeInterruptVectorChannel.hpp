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
    ZcodeInterruptVectorManager<ZP> *vectorManager;
    ZcodeBusInterrupt<ZP> interrupt;
    ZcodeInterruptVectorChannelIn<ZP> inA;
    ZcodeInterruptVectorOut<ZP> outA;

public:
    ZcodeInterruptVectorChannel(Zcode<ZP> *zcode, ZcodeInterruptVectorManager<ZP> *vectorManager, ZcodeNotificationManager<ZP> *notificationManager) :
            ZcodeCommandChannel<ZP>(zcode, &inA, &outA, false), vectorManager(vectorManager), inA(zcode, this), outA(notificationManager) {
    }

    void setInterrupt(ZcodeBusInterrupt<ZP> interrupt) {
        this->interrupt = interrupt;
        inA.start(vectorManager->findVector(&interrupt));
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
        ZcodeOutStream<TestParams> *out = slot.getOut();
        out->writeField16('B', ZP::scriptBigSize);
        out->writeField16('F', TestParams::fieldNum);
        out->writeField8('M', 2);
        out->writeBigStringField(ZP::Strings::interruptVectorChannelDescriptor);
        out->writeStatus(OK);
    }

    void readSetup(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<TestParams> *out = slot.getOut();
        out->writeStatus(OK);
        //TODO: proper setup
    }
};
#include "../scriptspace/ZcodeInterruptVectorManager.hpp"

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORCHANNEL_HPP_ */

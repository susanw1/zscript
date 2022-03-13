/*
 * ZcodeInterruptVectorChannel.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORCHANNEL_HPP_
#define SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORCHANNEL_HPP_
#include "../ZcodeIncludes.hpp"
#include "../parsing/ZcodeCommandChannel.hpp"
#include "../parsing/ZcodeCommandSequence.hpp"
#include "../ZcodeLockSet.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeInterruptVectorManager;

template<class ZP>
class ZcodeBusInterrupt;

template<class ZP>
class ZcodeExecutionSpace;

template<class ZP>
class ZcodeExecutionSpaceChannelIn;

template<class ZP>
class ZcodeInterruptVectorChannel: public ZcodeCommandChannel<ZP> {
private:
    Zcode<ZP> *zcode;
    ZcodeExecutionSpace<ZP> *space;
    ZcodeInterruptVectorManager<ZP> *vectorManager;
    ZcodeCommandSequence<ZP> sequence;
    ZcodeBusInterrupt<ZP> interrupt;
    ZcodeExecutionSpaceChannelIn<ZP> *in = NULL;
    ZcodeLockSet<ZP> locks;

public:
    ZcodeInterruptVectorChannel(ZcodeExecutionSpace<ZP> *space, ZcodeInterruptVectorManager<ZP> *vectorManager, Zcode<ZP> *z) :
            zcode(z), space(space), vectorManager(vectorManager), sequence(z, this) {
        locks.addLock(ZP::executionSpaceLock, false);
    }

    ZcodeChannelInStream<ZP>* acquireInStream();

    bool hasInStream() {
        return in != NULL;
    }

    ZcodeOutStream<ZP>* acquireOutStream();

    bool hasOutStream() {
        return true;
    }
    bool hasCommandSequence();

    ZcodeCommandSequence<ZP>* getCommandSequence() {
        return &sequence;
    }

    bool isPacketBased() {
        return false;
    }

    void releaseInStream() {
        if (in != NULL) {
            space->releaseInStream(in);
        }
        in = NULL;
    }

    void releaseOutStream() {
        interrupt = ZcodeBusInterrupt<ZP>();
    }

    ZcodeBusInterrupt<ZP>* getInterrupt() {
        return &interrupt;
    }

    void setAsNotificationChannel() {
    }

    void releaseFromNotificationChannel() {
    }

    void setAsDebugChannel() {
    }

    void releaseFromDebugChannel() {
    }

    void lock();

    bool canLock();

    void unlock();

};
#include "ZcodeInterruptVectorManager.hpp"

template<class ZP>
ZcodeChannelInStream<ZP>* ZcodeInterruptVectorChannel<ZP>::acquireInStream() {
    if (interrupt.getSource() == NULL) {
        interrupt = vectorManager->takeInterrupt();
    }
    if (in == NULL) {
        in = space->acquireInStream(vectorManager->findVector(&interrupt));
    }
    return in;
}

template<class ZP>
ZcodeOutStream<ZP>* ZcodeInterruptVectorChannel<ZP>::acquireOutStream() {
    return vectorManager->getOut();
}

template<class ZP>
bool ZcodeInterruptVectorChannel<ZP>::hasCommandSequence() {
    return interrupt.getSource() == NULL && vectorManager->hasInterruptSource()
            && space->hasInStream();
}

template<class ZP>
void ZcodeInterruptVectorChannel<ZP>::lock() {
    zcode->lock(&locks);
}

template<class ZP>
bool ZcodeInterruptVectorChannel<ZP>::canLock() {
    return zcode->canLock(&locks);
}

template<class ZP>
void ZcodeInterruptVectorChannel<ZP>::unlock() {
    zcode->unlock(&locks);
}

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORCHANNEL_HPP_ */

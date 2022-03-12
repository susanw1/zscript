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

template<class RP>
class Zcode;

template<class RP>
class ZcodeInterruptVectorManager;

template<class RP>
class ZcodeBusInterrupt;

template<class RP>
class ZcodeExecutionSpace;

template<class RP>
class ZcodeExecutionSpaceChannelIn;

template<class RP>
class ZcodeInterruptVectorChannel: public ZcodeCommandChannel<RP> {
private:
    Zcode<RP> *zcode;
    ZcodeExecutionSpace<RP> *space;
    ZcodeInterruptVectorManager<RP> *vectorManager;
    ZcodeCommandSequence<RP> sequence;
    ZcodeBusInterrupt<RP> interrupt;
    ZcodeExecutionSpaceChannelIn<RP> *in = NULL;
    ZcodeLockSet<RP> locks;

public:
    ZcodeInterruptVectorChannel(ZcodeExecutionSpace<RP> *space, ZcodeInterruptVectorManager<RP> *vectorManager, Zcode<RP> *z) :
            zcode(z), space(space), vectorManager(vectorManager), sequence(z, this) {
        locks.addLock(RP::executionSpaceLock, false);
    }

    ZcodeChannelInStream<RP>* acquireInStream();

    bool hasInStream() {
        return in != NULL;
    }

    ZcodeOutStream<RP>* acquireOutStream();

    bool hasOutStream() {
        return true;
    }
    bool hasCommandSequence();

    ZcodeCommandSequence<RP>* getCommandSequence() {
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
        interrupt = ZcodeBusInterrupt<RP>();
    }

    ZcodeBusInterrupt<RP>* getInterrupt() {
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

template<class RP>
ZcodeChannelInStream<RP>* ZcodeInterruptVectorChannel<RP>::acquireInStream() {
    if (interrupt.getSource() == NULL) {
        interrupt = vectorManager->takeInterrupt();
    }
    if (in == NULL) {
        in = space->acquireInStream(vectorManager->findVector(&interrupt));
    }
    return in;
}

template<class RP>
ZcodeOutStream<RP>* ZcodeInterruptVectorChannel<RP>::acquireOutStream() {
    return vectorManager->getOut();
}

template<class RP>
bool ZcodeInterruptVectorChannel<RP>::hasCommandSequence() {
    return interrupt.getSource() == NULL && vectorManager->hasInterruptSource()
            && space->hasInStream();
}

template<class RP>
void ZcodeInterruptVectorChannel<RP>::lock() {
    zcode->lock(&locks);
}

template<class RP>
bool ZcodeInterruptVectorChannel<RP>::canLock() {
    return zcode->canLock(&locks);
}

template<class RP>
void ZcodeInterruptVectorChannel<RP>::unlock() {
    zcode->unlock(&locks);
}

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORCHANNEL_HPP_ */

/*
 * RCodeInterruptVectorChannel.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORCHANNEL_HPP_
#define SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORCHANNEL_HPP_
#include "../RCodeIncludes.hpp"
#include "../parsing/RCodeCommandChannel.hpp"
#include "../parsing/RCodeCommandSequence.hpp"
#include "../RCodeLockSet.hpp"

template<class RP>
class RCode;

template<class RP>
class RCodeInterruptVectorManager;

template<class RP>
class RCodeBusInterrupt;

template<class RP>
class RCodeExecutionSpace;

template<class RP>
class RCodeExecutionSpaceChannelIn;

template<class RP>
class RCodeInterruptVectorChannel: public RCodeCommandChannel<RP> {
private:
    RCode<RP> *rcode;
    RCodeExecutionSpace<RP> *space;
    RCodeInterruptVectorManager<RP> *vectorManager;
    RCodeCommandSequence<RP> sequence;
    RCodeBusInterrupt<RP> interrupt;
    RCodeExecutionSpaceChannelIn<RP> *in = NULL;
    RCodeLockSet<RP> locks;

public:
    RCodeInterruptVectorChannel(RCodeExecutionSpace<RP> *space, RCodeInterruptVectorManager<RP> *vectorManager, RCode<RP> *r) :
            rcode(r), space(space), vectorManager(vectorManager), sequence(r, this) {
        locks.addLock(RP::executionSpaceLock, false);
    }

    RCodeChannelInStream<RP>* acquireInStream();

    bool hasInStream() {
        return in != NULL;
    }

    RCodeOutStream<RP>* acquireOutStream();

    bool hasOutStream() {
        return true;
    }
    bool hasCommandSequence();

    RCodeCommandSequence<RP>* getCommandSequence() {
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
        interrupt = RCodeBusInterrupt<RP>();
    }

    RCodeBusInterrupt<RP>* getInterrupt() {
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
#include "RCodeInterruptVectorManager.hpp"

template<class RP>
RCodeChannelInStream<RP>* RCodeInterruptVectorChannel<RP>::acquireInStream() {
    if (interrupt.getSource() == NULL) {
        interrupt = vectorManager->takeInterrupt();
    }
    if (in == NULL) {
        in = space->acquireInStream(vectorManager->findVector(&interrupt));
    }
    return in;
}

template<class RP>
RCodeOutStream<RP>* RCodeInterruptVectorChannel<RP>::acquireOutStream() {
    return vectorManager->getOut();
}

template<class RP>
bool RCodeInterruptVectorChannel<RP>::hasCommandSequence() {
    return interrupt.getSource() == NULL && vectorManager->hasInterruptSource()
            && space->hasInStream();
}

template<class RP>
void RCodeInterruptVectorChannel<RP>::lock() {
    rcode->lock(&locks);
}

template<class RP>
bool RCodeInterruptVectorChannel<RP>::canLock() {
    return rcode->canLock(&locks);
}

template<class RP>
void RCodeInterruptVectorChannel<RP>::unlock() {
    rcode->unlock(&locks);
}

#endif /* SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORCHANNEL_HPP_ */

/*
 * RCodeExecutionSpaceChannel.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACECHANNEL_HPP_
#define SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACECHANNEL_HPP_

#include "../parsing/RCodeCommandSequence.hpp"
#include "../parsing/RCodeCommandChannel.hpp"
#include "RCodeExecutionSpaceChannelIn.hpp"

template<class RP>
class RCode;

template<class RP>
class RCodeExecutionSpace;

template<class RP>
class RCodeExecutionSpaceChannel: public RCodeCommandChannel<RP> {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
private:
    RCode<RP> *rcode;
    RCodeExecutionSpace<RP> *space;
    RCodeCommandSequence<RP> sequence;
    RCodeExecutionSpaceChannelIn<RP> *in;
    RCodeOutStream<RP> *out = NULL;
    RCodeLockSet<RP> locks;
    executionSpaceAddress_t position = 0;
    uint8_t delayCounter = 0;
    uint8_t lockCount = 0;
public:
    RCodeExecutionSpaceChannel(RCode<RP> *rcode, RCodeExecutionSpace<RP> *space) :
            rcode(rcode), space(space), sequence(rcode, this), in(NULL) {
        locks.addLock(RP::executionSpaceLock, false);
    }

    RCodeCommandSequence<RP>* getCommandSequence() {
        return &sequence;
    }

    void move(executionSpaceAddress_t position) {
        this->position = position;
    }

    bool isPacketBased() {
        return false;
    }
    RCodeExecutionSpaceChannelIn<RP>* acquireInStream() {
        if (this->in == NULL) {
            in = space->acquireInStream(position);
        }
        return in;
    }

    bool hasInStream() {
        return in != NULL;
    }

    RCodeOutStream<RP>* acquireOutStream() {
        if (out == NULL) {
            out = space->acquireOutStream();
        }
        return out;
    }

    bool hasOutStream() {
        return out != NULL;
    }

    bool hasCommandSequence() {
        bool has = space->isRunning() && space->hasInStream() && space->hasOutStream() && in == NULL && out == NULL && delayCounter >= space->getDelay();
        delayCounter++;
        return has;
    }

    void releaseInStream() {
        delayCounter = 0;
        if (in != NULL) {
            position = (executionSpaceAddress_t) (in->getPosition() + 1);
            if (position >= space->getLength()) {
                position = 0;
            }
            space->releaseInStream(in);
        }
        in = NULL;
    }

    void releaseOutStream() {
        space->releaseOutStream((RCodeExecutionSpaceOut<RP>*) out);
        out = NULL;
    }

    void lock() {
        if (lockCount == 0) {
            rcode->lock(&locks);
        }
        lockCount++;
    }

    bool canLock() {
        return lockCount > 0 || rcode->canLock(&locks);
    }

    void unlock() {
        lockCount--;
        if (lockCount == 0) {
            rcode->unlock(&locks);
        }
    }

    void setAsNotificationChannel() {
    }

    void releaseFromNotificationChannel() {
    }

    void setAsDebugChannel() {
    }

    void releaseFromDebugChannel() {
    }
};

#include "../RCode.hpp"
#include "RCodeExecutionSpace.hpp"

#endif /* SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACECHANNEL_HPP_ */

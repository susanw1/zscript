/*
 * ZcodeExecutionSpaceChannel.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_

#include "../parsing/ZcodeCommandSequence.hpp"
#include "../parsing/ZcodeCommandChannel.hpp"
#include "ZcodeExecutionSpaceChannelIn.hpp"

template<class RP>
class Zcode;

template<class RP>
class ZcodeExecutionSpace;

template<class RP>
class ZcodeExecutionSpaceChannel: public ZcodeCommandChannel<RP> {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
private:
    Zcode<RP> *zcode;
    ZcodeExecutionSpace<RP> *space;
    ZcodeCommandSequence<RP> sequence;
    ZcodeExecutionSpaceChannelIn<RP> *in;
    ZcodeOutStream<RP> *out = NULL;
    ZcodeLockSet<RP> locks;
    executionSpaceAddress_t position = 0;
    uint8_t delayCounter = 0;
    uint8_t lockCount = 0;
public:
    ZcodeExecutionSpaceChannel(Zcode<RP> *zcode, ZcodeExecutionSpace<RP> *space) :
            zcode(zcode), space(space), sequence(zcode, this), in(NULL) {
        locks.addLock(RP::executionSpaceLock, false);
    }

    ZcodeCommandSequence<RP>* getCommandSequence() {
        return &sequence;
    }

    void move(executionSpaceAddress_t position) {
        this->position = position;
    }

    bool isPacketBased() {
        return false;
    }
    ZcodeExecutionSpaceChannelIn<RP>* acquireInStream() {
        if (this->in == NULL) {
            in = space->acquireInStream(position);
        }
        return in;
    }

    bool hasInStream() {
        return in != NULL;
    }

    ZcodeOutStream<RP>* acquireOutStream() {
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
        space->releaseOutStream((ZcodeExecutionSpaceOut<RP>*) out);
        out = NULL;
    }

    void lock() {
        if (lockCount == 0) {
            zcode->lock(&locks);
        }
        lockCount++;
    }

    bool canLock() {
        return lockCount > 0 || zcode->canLock(&locks);
    }

    void unlock() {
        lockCount--;
        if (lockCount == 0) {
            zcode->unlock(&locks);
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

#include "../Zcode.hpp"
#include "ZcodeExecutionSpace.hpp"

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACECHANNEL_HPP_ */

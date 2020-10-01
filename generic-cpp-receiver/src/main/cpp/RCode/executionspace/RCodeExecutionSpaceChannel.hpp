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

class RCode;
class RCodeExecutionSpace;

class RCodeExecutionSpaceChannel: public RCodeCommandChannel {
private:
    RCode *rcode;
    RCodeExecutionSpace *space;
    RCodeCommandSequence sequence;
    RCodeInStream in;
    RCodeOutStream *out = NULL;
    RCodeLockSet locks;
    executionSpaceAddress_t position = 0;
    uint8_t delayCounter = 0;
    uint8_t lockCount = 0;
public:
    RCodeExecutionSpaceChannel(RCode *rcode, RCodeExecutionSpace *space) :
            rcode(rcode), space(space), sequence(rcode, this), in(NULL) {
        locks.addLock(RCodeLockValues::executionSpaceLock, false);
    }
    RCodeCommandSequence* getCommandSequence() {
        return &sequence;
    }
    void move(executionSpaceAddress_t position) {
        this->position = position;
    }

    bool isPacketBased() {
        return false;
    }
    RCodeInStream* getInStream();

    RCodeOutStream* getOutStream();

    bool hasCommandSequence();

    void releaseInStream();

    void releaseOutStream();

    void lock();

    bool canLock();

    void unlock();

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

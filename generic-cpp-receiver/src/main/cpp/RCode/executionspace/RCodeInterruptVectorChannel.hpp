/*
 * RCodeInterruptVectorChannel.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORCHANNEL_HPP_
#define SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORCHANNEL_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "../parsing/RCodeCommandChannel.hpp"
#include "../parsing/RCodeCommandSequence.hpp"
#include "../RCodeLockSet.hpp"

class RCode;
class RCodeInterruptVectorManager;
class RCodeBusInterrupt;
class RCodeExecutionSpaceChannelIn;

class RCodeInterruptVectorChannel: public RCodeCommandChannel {
private:
    RCode *rcode;
    RCodeExecutionSpace *space;
    RCodeInterruptVectorManager *vectorManager;
    RCodeCommandSequence sequence;
    RCodeBusInterrupt *interrupt = NULL;
    RCodeExecutionSpaceChannelIn *in = NULL;
    RCodeLockSet locks;

public:
    RCodeInterruptVectorChannel(RCodeExecutionSpace *space,
            RCodeInterruptVectorManager *vectorManager, RCode *r) :
            rcode(r), space(space), vectorManager(vectorManager), sequence(r,
                    this) {
    }

    RCodeChannelInStream* acquireInStream();

    bool hasInStream() {
        return in != NULL;
    }

    RCodeOutStream* acquireOutStream();

    bool hasOutStream() {
        return true;
    }
    bool hasCommandSequence();

    RCodeCommandSequence* getCommandSequence() {
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
        interrupt = NULL;
    }

    RCodeBusInterrupt* getInterrupt() {
        return interrupt;
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

#endif /* SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORCHANNEL_HPP_ */

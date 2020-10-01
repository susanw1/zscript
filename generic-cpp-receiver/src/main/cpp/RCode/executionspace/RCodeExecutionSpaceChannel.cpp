/*
 * RCodeExecutionSpaceChannel.cpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionSpaceChannel.hpp"

RCodeInStream* RCodeExecutionSpaceChannel::getInStream() {
    if (in.getSequenceIn() == NULL) {
        in = RCodeInStream(space->acquireInStream(position));
    }
    return &in;
}

RCodeOutStream* RCodeExecutionSpaceChannel::getOutStream() {
    if (out == NULL) {
        out = space->acquireOutStream();
    }
    return out;
}

bool RCodeExecutionSpaceChannel::hasCommandSequence() {
    bool has = space->isRunning() && space->hasInStream()
            && space->hasOutStream() && in.getSequenceIn() == NULL
            && out == NULL && delayCounter >= space->getDelay();
    if (has) {
        in = RCodeInStream(space->acquireInStream(position));
        out = space->acquireOutStream();
    }
    delayCounter++;
    return has;
}

void RCodeExecutionSpaceChannel::releaseInStream() {
    delayCounter = 0;
    if (in.getSequenceIn() != NULL) {
        position =
                ((RCodeExecutionSpaceSequenceIn*) in.getSequenceIn())->getPosition();
        space->releaseInStream(
                (RCodeExecutionSpaceSequenceIn*) in.getSequenceIn());
    }
    in = RCodeInStream(NULL);
}

void RCodeExecutionSpaceChannel::releaseOutStream() {
    space->releaseOutStream((RCodeExecutionSpaceOut*) out);
    out = NULL;
}

void RCodeExecutionSpaceChannel::lock() {
    if (lockCount == 0) {
        rcode->lock(&locks);
    }
    lockCount++;
}

bool RCodeExecutionSpaceChannel::canLock() {
    return lockCount > 0 || rcode->canLock(&locks);
}

void RCodeExecutionSpaceChannel::unlock() {
    lockCount--;
    if (lockCount == 0) {
        rcode->unlock(&locks);
    }
}

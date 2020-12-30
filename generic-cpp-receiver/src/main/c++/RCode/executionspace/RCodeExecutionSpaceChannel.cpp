/*
 * RCodeExecutionSpaceChannel.cpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionSpaceChannel.hpp"

RCodeExecutionSpaceChannelIn* RCodeExecutionSpaceChannel::acquireInStream() {
    if (in == NULL) {
        in = space->acquireInStream(position);
    }
    return in;
}

RCodeOutStream* RCodeExecutionSpaceChannel::acquireOutStream() {
    if (out == NULL) {
        out = space->acquireOutStream();
    }
    return out;
}

bool RCodeExecutionSpaceChannel::hasCommandSequence() {
    bool has = space->isRunning() && space->hasInStream()
            && space->hasOutStream() && in == NULL && out == NULL
            && delayCounter >= space->getDelay();
    delayCounter++;
    return has;
}

void RCodeExecutionSpaceChannel::releaseInStream() {
    delayCounter = 0;
    if (in != NULL) {
        position = in->getPosition() + 1;
        if (position >= space->getLength()) {
            position = 0;
        }
        space->releaseInStream(in);
    }
    in = NULL;
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

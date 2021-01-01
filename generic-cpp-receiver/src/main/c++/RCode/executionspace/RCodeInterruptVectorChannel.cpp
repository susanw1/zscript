/*
 * RCodeInterruptVectorChannel.cpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#include "RCodeInterruptVectorChannel.hpp"

RCodeChannelInStream* RCodeInterruptVectorChannel::acquireInStream() {
    if (interrupt.getSource() == NULL) {
        interrupt = vectorManager->takeInterrupt();
    }
    if (in == NULL) {
        in = space->acquireInStream(vectorManager->findVector(&interrupt));
    }
    return in;
}
RCodeOutStream* RCodeInterruptVectorChannel::acquireOutStream() {
    return vectorManager->getOut();
}

bool RCodeInterruptVectorChannel::hasCommandSequence() {
    return interrupt.getSource() == NULL && vectorManager->hasInterruptSource()
            && space->hasInStream();
}

void RCodeInterruptVectorChannel::lock() {
    rcode->lock(&locks);
}

bool RCodeInterruptVectorChannel::canLock() {
    return rcode->canLock(&locks);
}

void RCodeInterruptVectorChannel::unlock() {
    rcode->unlock(&locks);
}

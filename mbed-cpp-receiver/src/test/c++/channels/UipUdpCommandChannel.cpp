/*
 * UipUdpCommandChannel.cpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#include "UipUdpCommandChannel.hpp"

UipUdpChannelInStream* UipUdpCommandChannel::acquireInStream() {
    return &in;
}

RCodeOutStream* UipUdpCommandChannel::acquireOutStream() {
    return &out;
}

bool UipUdpCommandChannel::hasCommandSequence() {
    if (in.hasNextCommandSequence()) {
        setAddress(in.getReader()->remoteIP().rawAddress(),
                in.getReader()->remotePort());
        return true;
    }
    return false;
}

void UipUdpCommandChannel::releaseInStream() {
    in.close();
}

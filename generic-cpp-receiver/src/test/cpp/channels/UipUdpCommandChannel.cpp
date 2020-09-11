/*
 * UipUdpCommandChannel.cpp
 *
 *  Created on: 10 Sep 2020
 *      Author: robert
 */

#include "UipUdpCommandChannel.hpp"

RCodeInStream* UipUdpCommandChannel::getInStream() {
    return manager->getInStream();
}

RCodeOutStream* UipUdpCommandChannel::getOutStream() {
    return manager->getOutStream();
}

bool UipUdpCommandChannel::hasCommandSequence() {
    manager->checkSequences();
    return hasSequence;
}

void UipUdpCommandChannel::releaseInStream() {
    manager->getInStream()->getSequenceIn()->closeCommandSequence();
    manager->inReleased();
}
void setupChannels(UipUdpChannelManager *manager, RCode *rcode) {
    for (int i = 0; i < RCodeParameters::uipChannelNum; i++) {
        manager->channels[i].setup(rcode, manager);
    }
}

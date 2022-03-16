/*
 * EthernetChannelInStream.cpp
 *
 *  Created on: 1 Jan 2021
 *      Author: robert
 */

#include "EthernetUdpChannelInStream.hpp"

int16_t EthernetUdpChannelInStream::read() {
    return channel->getUdp()->read();
}
char EthernetUdpChannelLookahead::read() {
    int result = parent->getChannel()->getUdp()->peek(dist++);
    if (result == -1) {
        return Zchars::EOL_SYMBOL;
    }
    return result;
}

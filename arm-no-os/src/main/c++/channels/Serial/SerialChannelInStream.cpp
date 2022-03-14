/*
 * SerialChannelInStream.cpp
 *
 *  Created on: 2 Aug 2021
 *      Author: robert
 */

#include "SerialChannelInStream.hpp"

int16_t SerialChannelInStream::read() {
    int16_t v = channel->getSerial()->read();
    if (v == '\n') {
        channel->hasNewline = channel->getSerial()->getDistance('\n') >= 0;
    }
    return v;
}
char SerialChannelLookahead::read() {
    int result = parent->getChannel()->getSerial()->peek();
    if (result == -1) {
        return '\n';
    }
    return result;
}

ZcodeLookaheadStream<ZcodeParameters>* SerialChannelInStream::getLookahead() {
    channel->getSerial()->resetPeek();
    return &lookahead;
}

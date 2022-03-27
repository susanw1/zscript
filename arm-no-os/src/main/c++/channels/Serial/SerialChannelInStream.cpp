/*
 * SerialChannelInStream.cpp
 *
 *  Created on: 2 Aug 2021
 *      Author: robert
 */

#include "SerialChannelInStream.hpp"

int16_t SerialChannelInStream::read() {
    int16_t v = channel->getSerial()->read();
    if (v == Zchars::EOL_SYMBOL) {
        channel->hasNewline = channel->getSerial()->getDistance(Zchars::EOL_SYMBOL) >= 0;
    }
    return v;
}
char SerialChannelLookahead::read() {
    int result = parent->getChannel()->getSerial()->peek();
    if (result == -1) {
        return Zchars::EOL_SYMBOL;
    }
    return result;
}

ZcodeLookaheadStream<ZcodeParameters>* SerialChannelInStream::getLookahead() {
    channel->getSerial()->resetPeek();
    return &lookahead;
}

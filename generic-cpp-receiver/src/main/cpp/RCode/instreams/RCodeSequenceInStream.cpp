/*
 * RCodeSequenceInStream.cpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#include "RCodeSequenceInStream.hpp"

void RCodeSequenceInStream::readInternal() {
    int next = channelIn->read();
    if (next == -1) {
        current = '\n';
        opened = false;
    } else {
        current = (char) next;
    }
}

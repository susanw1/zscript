/*
 * RCodeMarkerInStream.cpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#include "RCodeMarkerInStream.hpp"

char RCodeMarkerInStream::read() {
    if (hasRead) {
        sequenceIn->read();
    }
    hasRead = true;
    return sequenceIn->peek();
}

char RCodeMarkerInStream::reread() {
    return sequenceIn->peek();
}

bool RCodeMarkerInStream::hasNext() {
    return sequenceIn->hasNext();
}

void RCodeMarkerInStream::close() {
}

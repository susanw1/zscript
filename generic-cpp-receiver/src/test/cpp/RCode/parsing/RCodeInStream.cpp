/*
 * RCodeInStream.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */
#include "RCodeInStream.hpp"

int16_t RCodeInStream::readInternal() {
    if (!hasReachedCommandEnd) {
        int16_t prev = current;
        if (sequenceIn->hasNext()) {
            current = sequenceIn->next();
            currentValid = true;
            if (current == '\n' || (current == ';' && !isInString)) {
                hasReachedCommandEnd = true;
                currentValid = false;
            } else if (current == '"') {
                if (!hasBackslash) {
                    isInString = !isInString;
                }
            } else if (isInString) {
                if (current == '\\') {
                    hasBackslash = !hasBackslash;
                } else {
                    hasBackslash = false;
                }
            }
        } else {
            currentValid = false;
        }
        return prev;
    } else {
        return current;
    }
}
RCodeLookaheadStream* RCodeInStream::getLookahead() {
    return sequenceIn->getLookahead();
}

bool RCodeInStream::hasNext() {
    return currentValid || (!hasReachedCommandEnd && sequenceIn->hasNext());
}

void RCodeInStream::closeCommand() {
    while (hasNext()) {
        read();
    }
    if (current != '\n' && current != ';') {
        current = sequenceIn->next();
    }
}

void RCodeInStream::skipSequence() {
    if (current != '\n') {
        sequenceIn->closeCommandSequence();
        current = -1;
    }
}

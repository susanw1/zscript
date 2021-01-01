/*
 * RCodeCommandInStream.cpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#include "RCodeCommandInStream.hpp"

char RCodeCommandLookaheadStream::read() {
    if (lookahead == 0) {
        return parent->read();
    }
    if (lookahead-- == 2) {
        return commandIn->current;
    } else {
        return commandIn->sequenceIn->peek();
    }
}
void RCodeCommandLookaheadStream::reset(RCodeCommandInStream *commandIn) {
    this->commandIn = commandIn;
    this->parent = commandIn->sequenceIn->getChannelInStream()->getLookahead();
    this->lookahead = 2;
}
void RCodeCommandInStream::readInternal() {
    current = sequenceIn->read();

    if (current == '\n' || (!inString && (current == '&' || current == '|'))) {
        opened = false;
    } else if (current == '"' && !backslash) {
        inString = !inString;
    } else if (inString) {
        if (current == '\\') {
            backslash = !backslash;
        } else {
            backslash = false;
        }
    }
}

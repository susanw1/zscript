/*
 * RCodeExecutionSpaceSequenceIn.cpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionSpaceSequenceIn.hpp"

void RCodeExecutionSpaceSequenceIn::RCodeExecutionSpaceLookahead::reset() {
    this->stillInSequence = parent->isSequenceOpen;
    offset = 0;
}
char RCodeExecutionSpaceSequenceIn::RCodeExecutionSpaceLookahead::read() {
    if (stillInSequence) {
        if (parent->pos + offset >= parent->space->getLength()
                || parent->space->get(parent->pos + offset) == '\n') {
            stillInSequence = false;
            return '\n';
        } else {
            return (char) parent->space->get(parent->pos + offset++);
        }
    } else {
        return '\n';
    }
}
char RCodeExecutionSpaceSequenceIn::next() {
    if (isSequenceOpen) {
        if (pos >= space->getLength() || space->get(pos) == '\n') {
            isSequenceOpen = false;
            return '\n';
        } else {
            return (char) space->get(pos++);
        }
    } else {
        return '\n';
    }
}

void RCodeExecutionSpaceSequenceIn::openCommandSequence() {
    isSequenceOpen = true;
    hasStarted = false;
    if (pos < space->getLength() && space->get(pos) == '\n') {
        pos++;
    }
    if (pos >= space->getLength()) {
        pos = 0;
    }
}

bool RCodeExecutionSpaceSequenceIn::needsDelayNext() {
    return pos >= space->getLength()
            || (pos == space->getLength() - 1 && space->get(pos) == '\n');
}

void RCodeExecutionSpaceSequenceIn::closeCommandSequence() {
    if (isSequenceOpen) {
        isSequenceOpen = false;
        while (pos < space->getLength() && space->get(pos++) != '\n') {
        }
    }
}

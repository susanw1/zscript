/*
 * RCodeExecutionSpaceSequenceIn.cpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionSpaceChannelIn.hpp"

void RCodeExecutionSpaceChannelIn::RCodeExecutionSpaceLookahead::reset() {
    offset = 0;
}
char RCodeExecutionSpaceChannelIn::RCodeExecutionSpaceLookahead::read() {
    if (parent->pos + offset >= parent->space->getLength()
            || parent->space->get(parent->pos + offset) == '\n') {
        return '\n';
    } else {
        return (char) parent->space->get(parent->pos + offset++);
    }
}
int16_t RCodeExecutionSpaceChannelIn::read() {
    if (pos >= space->getLength() || space->get(pos) == '\n') {
        return -1;
    } else {
        return (char) space->get(pos++);
    }
}

bool RCodeExecutionSpaceChannelIn::needsDelayNext() {
    return pos >= space->getLength()
            || (pos == space->getLength() - 1 && space->get(pos) == '\n');
}

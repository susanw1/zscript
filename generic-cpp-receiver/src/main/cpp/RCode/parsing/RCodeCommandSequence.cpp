/*
 * RCodeCommandSequence.cpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#include "RCodeCommandSequence.hpp"

RCodeInStream* RCodeCommandSequence::getInStream() {
    if (in == NULL) {
        in = channel->getInStream();
        in->lock();
    }
    return in;
}
void RCodeCommandSequence::releaseInStream() {
    if (in != NULL) {
        in->unlock();
    }
    channel->releaseInStream();
    in = NULL;
}
RCodeOutStream* RCodeCommandSequence::getOutStream() {
    if (out == NULL) {
        out = channel->getOutStream();
        out->lock();
    }
    return out;
}

void RCodeCommandSequence::releaseOutStream() {
    out->unlock();
    channel->releaseOutStream();
    out = NULL;
}
void RCodeCommandSequence::addLast(RCodeCommandSlot *slot) {
    if (last != NULL) {
        last->next = slot;
        last = slot;
        if (locks.isActive()) {
            slot->getCommand(rcode)->setLocks(&locks);
        }
    } else {
        last = slot;
        first = slot;
    }
}

RCodeCommandSlot* RCodeCommandSequence::popFirst() {
    RCodeCommandSlot *target = first;
    if (first == NULL) {
        return NULL;
    } else if (first->next != NULL) {
        first = first->next;
    } else {
        last = NULL;
        first = NULL;
    }
    return target;
}

bool RCodeCommandSequence::fail(RCodeResponseStatus status) {
    if (status != CMD_FAIL) {
        RCodeCommandSlot *next = NULL;
        for (RCodeCommandSlot *current = first; current != NULL; current =
                next) {
            next = current->next;
            current->reset();
        }
        if (in != NULL) {
            in->skipSequence();
        }
        if (!fullyParsed) {
            failed = true;
            fullyParsed = true;
        }
        first = NULL;
        last = NULL;
        locks.reset();
        return false;
    } else {
        bool found = false;
        RCodeCommandSlot *next = NULL;
        for (RCodeCommandSlot *current = first; current != NULL; current =
                next) {
            next = current->next;
            if (current->getEnd() == '|') {
                found = true;
            }
            current->reset();
            if (found) {
                first = next;
                break;
            }
        }
        if (!found || first == NULL) {
            first = NULL;
            last = NULL;
            if (fullyParsed) {
                locks.reset();
                return false;
            } else {
                in->skipToError();
                if (in->read() == '\n') {
                    if (!fullyParsed) {
                        failed = true;
                        fullyParsed = true;
                    }
                    locks.reset();
                    return false;
                }
                return true;
            }
        } else {
            return true;
        }
    }
}
bool RCodeCommandSequence::parseFlags() {
    in->openCommand();
    if (RCodeParser::shouldEatWhitespace(in)) {
        RCodeParser::eatWhitespace(in);
    }
    if (in->peek() == '#') {
        in->skipSequence();
        return false;
    }
    if (in->peek() == '*') {
        in->read();
        broadcast = true;
        if (RCodeParser::shouldEatWhitespace(in)) {
            RCodeParser::eatWhitespace(in);
        }
    }
    if (in->peek() == '%') {
        in->read();
        parallel = true;
        if (RCodeParser::shouldEatWhitespace(in)) {
            RCodeParser::eatWhitespace(in);
        }
    }
    if (!in->getSequenceIn()->hasNext()) {
        empty = true;
        parallel = true;
        fullyParsed = true;
    }

    in->unOpen();
    return true;
}
bool RCodeCommandSequence::canLock() {
    if (!locks.isActive()) {
        locks.activate();
        for (RCodeCommandSlot *slot = first; slot != NULL; slot = slot->next) {
            if (slot->getCommand(rcode) != NULL) {
                slot->getCommand(rcode)->setLocks(&locks);
            }
        }
    }
    return rcode->canLock(&locks);
}
void RCodeCommandSequence::lock() {
    if (!locks.isActive()) {
        locks.activate();
        for (RCodeCommandSlot *slot = first; slot != NULL; slot = slot->next) {
            if (slot->getCommand(rcode) != NULL) {
                slot->getCommand(rcode)->setLocks(&locks);
            }
        }
    }
    rcode->lock(&locks);
}

void RCodeCommandSequence::unlock() {
    if (!locks.isActive()) {
        rcode->unlock(&locks);
        locks.reset();
    }
}

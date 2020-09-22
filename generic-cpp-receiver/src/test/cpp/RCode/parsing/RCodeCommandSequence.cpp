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
    in->unlock();
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

void RCodeCommandSequence::fail() {
    for (RCodeCommandSlot *current = first; current != NULL;
            current = current->next) {
        current->reset();
    }
    if (in != NULL) {
        in->skipSequence();
    }
    last = first;
    locks.reset();
}
bool RCodeCommandSequence::canLock() {
    return true;
    if (!locks.isActive()) {
        locks.activate();
        for (RCodeCommandSlot *slot = first; slot != NULL; slot = slot->next) {
            if (slot->getCommand(rcode) != NULL) {
                slot->getCommand(rcode)->setLocks(&locks);
            }
        }
        channel->setLocks(&locks);
    }
    return rcode->canLock(&locks);
}
void RCodeCommandSequence::lock() {
    return;
    if (!locks.isActive()) {
        locks.activate();
        for (RCodeCommandSlot *slot = first; slot != NULL; slot = slot->next) {
            if (slot->getCommand(rcode) != NULL) {
                slot->getCommand(rcode)->setLocks(&locks);
            }
        }
        channel->setLocks(&locks);
    }
    rcode->lock(&locks);
}

void RCodeCommandSequence::unlock() {
    return;
    if (!locks.isActive()) {
        rcode->unlock(&locks);
        locks.reset();
    }
}

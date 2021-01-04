/*
 * RCodeCommandSequence.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSEQUENCE_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSEQUENCE_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeLockSet.hpp"
#include "../instreams/RCodeMarkerInStream.hpp"

template<class RP>
class RCode;

template<class RP>
class RCodeCommandSlot;

template<class RP>
class RCodeCommandChannel;

template<class RP>
class RCodeSequenceInStream;

template<class RP>
class RCodeOutStream;

template<class RP>
class RCodeCommandSequence {
private:
    RCode<RP> *const rcode;
    RCodeCommandChannel<RP> *const channel;

    RCodeSequenceInStream<RP> *in = NULL;
    RCodeOutStream<RP> *out = NULL;
    RCodeLockSet<RP> locks;

    RCodeCommandSlot<RP> *first = NULL;
    RCodeCommandSlot<RP> *last = NULL;
    bool parallel = false;
    bool broadcast = false;

    bool fullyParsed = false;
    bool active = false;
    bool failed = false;
    bool empty = false;

public:
    RCodeCommandSequence(RCode<RP> *rcode, RCodeCommandChannel<RP> *channel) :
            rcode(rcode), channel(channel), locks() {
    }

    RCode<RP>* getRCode() {
        return rcode;
    }

    RCodeSequenceInStream<RP>* acquireInStream();

    void releaseInStream();

    RCodeOutStream<RP>* acquireOutStream();

    void releaseOutStream();

    bool isEmpty() {
        return empty;
    }

    bool canBeParallel() const {
        return parallel;
    }
    bool isBroadcast() const {
        return broadcast;
    }
    void setFullyParsed(bool b) {
        fullyParsed = b;
    }

    bool isFullyParsed() const {
        return fullyParsed;
    }

    bool hasParsed() const {
        return first != NULL;
    }

    void setFailed() {
        failed = true;
    }
    void unsetFailed() {
        failed = false;
    }
    bool hasFailed() const {
        return failed;
    }
    bool canContinueParse() {
        return active && !fullyParsed && !failed;
    }

    void setActive() {
        active = true;
    }

    bool isActive() const {
        return active;
    }

    void addLast(RCodeCommandSlot<RP> *slot);

    RCodeCommandSlot<RP>* popFirst();

    RCodeCommandSlot<RP>* peekFirst() const {
        return first;
    }

    RCodeCommandChannel<RP>* getChannel() const {
        return channel;
    }

    void reset() {
        broadcast = false;
        parallel = false;
        active = false;
        last = NULL;
        first = NULL;
        fullyParsed = false;
        if (locks.isActive()) {
            unlock();
        }
        locks.reset();
        empty = false;
    }

    bool fail(RCodeResponseStatus status);

    bool parseFlags();

    bool canLock();

    void lock();

    void unlock();
};

template<class RP>
RCodeSequenceInStream<RP>* RCodeCommandSequence<RP>::acquireInStream() {
    if (in == NULL) {
        in = channel->acquireInStream()->getSequenceInStream();
        in->lock();
    }
    return in;
}

template<class RP>
void RCodeCommandSequence<RP>::releaseInStream() {
    if (in != NULL) {
        in->unlock();
    }
    channel->releaseInStream();
    in = NULL;
}

template<class RP>
RCodeOutStream<RP>* RCodeCommandSequence<RP>::acquireOutStream() {
    if (out == NULL) {
        out = channel->acquireOutStream();
        out->lock();
    }
    return out;
}

template<class RP>
void RCodeCommandSequence<RP>::releaseOutStream() {
    out->unlock();
    channel->releaseOutStream();
    out = NULL;
}

template<class RP>
void RCodeCommandSequence<RP>::addLast(RCodeCommandSlot<RP> *slot) {
    if (last != NULL) {
        last->next = slot;
        last = slot;
        if (locks.isActive()) {
            slot->getCommand(rcode)->setLocks(slot, &locks);
        }
    } else {
        last = slot;
        first = slot;
    }
}

template<class RP>
RCodeCommandSlot<RP>* RCodeCommandSequence<RP>::popFirst() {
    RCodeCommandSlot<RP> *target = first;
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

template<class RP>
bool RCodeCommandSequence<RP>::fail(RCodeResponseStatus status) {
    if (status != CMD_FAIL) {
        RCodeCommandSlot<RP> *next = NULL;
        for (RCodeCommandSlot<RP> *current = first; current != NULL; current = next) {
            next = current->next;
            current->reset();
        }
        if (in != NULL) {
            in->close();
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
        RCodeCommandSlot<RP> *next = NULL;
        for (RCodeCommandSlot<RP> *current = first; current != NULL; current = next) {
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
                if (in->peek() == '\n') {
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

template<class RP>
bool RCodeCommandSequence<RP>::parseFlags() {
    RCodeMarkerInStream<RP> *mIn = in->getMarkerInStream();
    mIn->eatWhitespace();
    mIn->read();
    if (mIn->reread() == '#') {
        mIn->close();
        in->close();
        return false;
    }
    if (mIn->reread() == '*') {
        mIn->read();
        broadcast = true;
        mIn->eatWhitespace();
    }
    if (mIn->reread() == '%') {
        mIn->read();
        parallel = true;
        mIn->eatWhitespace();
    }
    if (mIn->reread() == '\n') {
        mIn->read();
        empty = true;
        parallel = true;
        fullyParsed = true;
    }

    mIn->close();
    return true;
}

template<class RP>
bool RCodeCommandSequence<RP>::canLock() {
    if (!locks.isActive()) {
        locks.activate();
        for (RCodeCommandSlot<RP> *slot = first; slot != NULL; slot = slot->next) {
            if (slot->getCommand(rcode) != NULL) {
                slot->getCommand(rcode)->setLocks(slot, &locks);
            }
        }
    }
    return rcode->canLock(&locks);
}

template<class RP>
void RCodeCommandSequence<RP>::lock() {
    if (!locks.isActive()) {
        locks.activate();
        for (RCodeCommandSlot<RP> *slot = first; slot != NULL; slot = slot->next) {
            if (slot->getCommand(rcode) != NULL) {
                slot->getCommand(rcode)->setLocks(slot, &locks);
            }
        }
    }
    rcode->lock(&locks);
}

template<class RP>
void RCodeCommandSequence<RP>::unlock() {
    if (!locks.isActive()) {
        rcode->unlock(&locks);
        locks.reset();
    }
}

#include "../instreams/RCodeSequenceInStream.hpp"
#include "../RCodeOutStream.hpp"
#include "RCodeCommandSlot.hpp"
#include "RCodeCommandChannel.hpp"
#include "../commands/RCodeCommand.hpp"
#include "../RCode.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSEQUENCE_HPP_ */

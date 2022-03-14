/*
 * ZcodeCommandSequence.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDSEQUENCE_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDSEQUENCE_HPP_
#include "../ZcodeIncludes.hpp"
#include "../ZcodeLockSet.hpp"
#include "../instreams/ZcodeMarkerInStream.hpp"
#include "../ZcodeAddressingCommandConsumer.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeCommandSlot;

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeSequenceInStream;

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeCommandSequence {
private:
    Zcode<ZP> *const zcode;
    ZcodeCommandChannel<ZP> *const channel;

    ZcodeSequenceInStream<ZP> *in = NULL;
    ZcodeOutStream<ZP> *out = NULL;
    ZcodeLockSet<ZP> locks;

    ZcodeCommandSlot<ZP> *first = NULL;
    ZcodeCommandSlot<ZP> *last = NULL;
    bool parallel = false;
    bool broadcast = false;

    bool fullyParsed = false;
    bool active = false;
    bool failed = false;
    bool empty = false;

public:
    ZcodeCommandSequence(Zcode<ZP> *zcode, ZcodeCommandChannel<ZP> *channel) :
            zcode(zcode), channel(channel), locks() {
    }

    Zcode<ZP>* getZcode() {
        return zcode;
    }

    ZcodeSequenceInStream<ZP>* acquireInStream();

    void releaseInStream();

    ZcodeOutStream<ZP>* acquireOutStream();

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

    void addLast(ZcodeCommandSlot<ZP> *slot);

    ZcodeCommandSlot<ZP>* popFirst();

    ZcodeCommandSlot<ZP>* peekFirst() const {
        return first;
    }

    ZcodeCommandChannel<ZP>* getChannel() const {
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

    bool fail(ZcodeResponseStatus status);

    bool parseFlags();

    bool canLock();

    void lock();

    void unlock();
};

template<class ZP>
ZcodeSequenceInStream<ZP>* ZcodeCommandSequence<ZP>::acquireInStream() {
    if (in == NULL) {
        in = channel->acquireInStream()->getSequenceInStream();
        in->lock();
    }
    return in;
}

template<class ZP>
void ZcodeCommandSequence<ZP>::releaseInStream() {
    if (in != NULL) {
        in->unlock();
    }
    channel->releaseInStream();
    in = NULL;
}

template<class ZP>
ZcodeOutStream<ZP>* ZcodeCommandSequence<ZP>::acquireOutStream() {
    if (out == NULL) {
        out = channel->acquireOutStream();
        out->lock();
    }
    return out;
}

template<class ZP>
void ZcodeCommandSequence<ZP>::releaseOutStream() {
    out->unlock();
    channel->releaseOutStream();
    out = NULL;
}

template<class ZP>
void ZcodeCommandSequence<ZP>::addLast(ZcodeCommandSlot<ZP> *slot) {
    if (last != NULL) {
        last->next = slot;
        last = slot;
        if (locks.isActive()) {
            slot->getCommand(zcode)->setLocks(slot, &locks);
        }
    } else {
        last = slot;
        first = slot;
    }
}

template<class ZP>
ZcodeCommandSlot<ZP>* ZcodeCommandSequence<ZP>::popFirst() {
    ZcodeCommandSlot<ZP> *target = first;
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

template<class ZP>
bool ZcodeCommandSequence<ZP>::fail(ZcodeResponseStatus status) {
    if (status != CMD_FAIL) {
        ZcodeCommandSlot<ZP> *next = NULL;
        for (ZcodeCommandSlot<ZP> *current = first; current != NULL; current = next) {
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
        ZcodeCommandSlot<ZP> *next = NULL;
        for (ZcodeCommandSlot<ZP> *current = first; current != NULL; current = next) {
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

template<class ZP>
bool ZcodeCommandSequence<ZP>::parseFlags() {
    ZcodeMarkerInStream<ZP> *mIn = in->getMarkerInStream();
    mIn->eatWhitespace();
    mIn->read();
    if (mIn->reread() == '@') {
        mIn->close();
        if (zcode->getAddressingCommandConsumer() != NULL) {
            zcode->getAddressingCommandConsumer()->consumeAddressedCommandSequence(in);
        }
        in->close();
        return false;
    }
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

template<class ZP>
bool ZcodeCommandSequence<ZP>::canLock() {
    if (!locks.isActive()) {
        locks.activate();
        for (ZcodeCommandSlot<ZP> *slot = first; slot != NULL; slot = slot->next) {
            if (slot->getCommand(zcode) != NULL) {
                slot->getCommand(zcode)->setLocks(slot, &locks);
            }
        }
    }
    return zcode->canLock(&locks);
}

template<class ZP>
void ZcodeCommandSequence<ZP>::lock() {
    if (!locks.isActive()) {
        locks.activate();
        for (ZcodeCommandSlot<ZP> *slot = first; slot != NULL; slot = slot->next) {
            if (slot->getCommand(zcode) != NULL) {
                slot->getCommand(zcode)->setLocks(slot, &locks);
            }
        }
    }
    zcode->lock(&locks);
}

template<class ZP>
void ZcodeCommandSequence<ZP>::unlock() {
    if (!locks.isActive()) {
        zcode->unlock(&locks);
        locks.reset();
    }
}

#include "../instreams/ZcodeSequenceInStream.hpp"
#include "../ZcodeOutStream.hpp"
#include "ZcodeCommandSlot.hpp"
#include "ZcodeCommandChannel.hpp"
#include "../commands/ZcodeCommand.hpp"
#include "../Zcode.hpp"

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECOMMANDSEQUENCE_HPP_ */

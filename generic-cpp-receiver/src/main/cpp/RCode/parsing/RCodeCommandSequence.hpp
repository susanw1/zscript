/*
 * RCodeCommandSequence.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSEQUENCE_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSEQUENCE_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "../RCodeLockSet.hpp"

class RCode;
class RCodeCommandSlot;
class RCodeCommandChannel;
class RCodeSequenceInStream;
class RCodeOutStream;

class RCodeCommandSequence {
private:
    RCode *rcode;
    RCodeCommandChannel *const channel;
    RCodeCommandSlot *first = NULL;
    RCodeCommandSlot *last = NULL;
    bool parallel = false;
    bool broadcast = false;
    RCodeSequenceInStream *in = NULL;
    RCodeOutStream *out = NULL;
    RCodeLockSet locks;

    bool fullyParsed = false;
    bool active = false;
    bool failed = false;
    bool empty = false;

public:
    RCodeCommandSequence(RCode *rcode, RCodeCommandChannel *channel) :
            rcode(rcode), channel(channel), locks() {
    }
    void setRCode(RCode *rcode) {
        this->rcode = rcode;
    }
    RCode* getRCode() {
        return rcode;
    }
    RCodeSequenceInStream* acquireInStream();

    void releaseInStream();

    RCodeOutStream* acquireOutStream();

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

    void addLast(RCodeCommandSlot *slot);

    RCodeCommandSlot* popFirst();

    RCodeCommandSlot* peekFirst() const {
        return first;
    }

    RCodeCommandChannel* getChannel() const {
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
}
;

#include "../instreams/RCodeSequenceInStream.hpp"
#include "../RCodeOutStream.hpp"
#include "RCodeCommandSlot.hpp"
#include "RCodeCommandChannel.hpp"
#include "../commands/RCodeCommand.hpp"
#include "../RCode.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSEQUENCE_HPP_ */

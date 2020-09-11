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
class RCodeInStream;
class RCodeOutStream;

class RCodeCommandSequence {
private:
    RCode *rcode;
    RCodeCommandChannel *const channel;
    bool fullyParsed = false;
    RCodeCommandSlot *first = NULL;
    RCodeCommandSlot *last = NULL;
    bool parallel = false;
    bool broadcast = false;
    RCodeInStream *in = NULL;
    RCodeOutStream *out = NULL;
    RCodeLockSet locks;
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
    RCodeInStream* getInStream();

    void releaseInStream();

    RCodeOutStream* getOutStream();

    void releaseOutStream();

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

    void addLast(RCodeCommandSlot *slot);

    RCodeCommandSlot* popFirst();

    RCodeCommandSlot* peekFirst() const {
        return first;
    }

    RCodeCommandChannel* getChannel() const {
        return channel;
    }

    void setBroadcast() {
        broadcast = true;
    }

    void setParallel() {
        parallel = true;
    }

    void reset() {
        broadcast = false;
        parallel = false;
        last = NULL;
        first = NULL;
        fullyParsed = false;
        locks.reset();
    }

    void fail();

    bool canLock();

    void lock();

    void unlock();
};

#include "RCodeInStream.hpp"
#include "../RCodeOutStream.hpp"
#include "RCodeCommandSlot.hpp"
#include "RCodeCommandChannel.hpp"
#include "../commands/RCodeCommand.hpp"
#include "../RCode.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODECOMMANDSEQUENCE_HPP_ */

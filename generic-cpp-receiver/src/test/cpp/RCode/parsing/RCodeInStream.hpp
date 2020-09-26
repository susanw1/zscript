/*
 * RCodeInStream.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODEINSTREAM_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODEINSTREAM_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "RCodeLookaheadStream.hpp"

class RCodeSequenceInStream;

class RCodeInStream {
private:
    bool hasReachedCommandEnd = false;
    bool isInString = false;
    int16_t current = -1;

    class RCodeInStreamLookaheadOffset: public RCodeLookaheadStream {
    private:
        RCodeLookaheadStream *toWrap = NULL;
        RCodeInStream *parent;
        bool hasUsed = false;

    public:
        RCodeInStreamLookaheadOffset(RCodeInStream *parent) :
                parent(parent) {
        }
        virtual void reset(RCodeLookaheadStream *toWrap) {
            hasUsed = false;
            this->toWrap = toWrap;
        }
        char read() {
            if (hasUsed || parent->current == -1) {
                return toWrap->read();
            }
            hasUsed = true;
            return (char) parent->current;
        }
    };
    RCodeInStreamLookaheadOffset lookahead;
    RCodeSequenceInStream *const sequenceIn;
    bool hasBackslash = false;
    bool lockVal = false;
    bool currentValid = false;

    int16_t readInternal();

public:
    RCodeInStream(RCodeSequenceInStream *sequenceIn) :
            lookahead(this), sequenceIn(sequenceIn) {
    }

    char read() {
        int16_t c = readInternal();
        if (c == -1) {
            return (char) readInternal();
        }
        return (char) c;
    }

    char peek() {
        if (current == -1) {
            readInternal();
        }
        return (char) current;
    }

    RCodeLookaheadStream* getLookahead();

    bool hasNext();

    void openCommand() {
        hasReachedCommandEnd = false;
        currentValid = true;
        if (current == '&' || current == '|' || current == '\n') {
            read();
        }
    }

    void closeCommand();

    bool isCommandOpen() const {
        return !hasReachedCommandEnd;
    }
    void skipToError();

    void skipSequence();

    void unlock() {
        lockVal = false;
    }

    bool lock() {
        if (lockVal) {
            return false;
        } else {
            lockVal = true;
            return true;
        }
    }

    bool isLocked() const {
        return lockVal;
    }

    RCodeSequenceInStream* getSequenceIn() {
        return sequenceIn;
    }
    void unOpen() {
        hasReachedCommandEnd = false;
        currentValid = true;
    }
};

#include "RCodeSequenceInStream.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODEINSTREAM_HPP_ */

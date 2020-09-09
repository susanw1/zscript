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

class RCodeLookaheadStream;
class RCodeSequenceInStream;

class RCodeInStream {
private:
    RCodeSequenceInStream *const sequenceIn;
    bool hasReachedCommandEnd = false;
    int16_t current = -1;
    bool isInString = false;
    bool hasBackslash = false;
    bool lockVal = false;
    bool currentValid = false;

    int16_t readInt();
public:
    RCodeInStream(RCodeSequenceInStream *sequenceIn) :
            sequenceIn(sequenceIn) {
    }

    char read() {
        int16_t c = readInt();
        if (c == -1) {
            return (char) readInt();
        }
        return (char) c;
    }

    char peek() {
        if (current == -1) {
            readInt();
        }
        return (char) current;
    }

    RCodeLookaheadStream* getLookahead();

    bool hasNext();

    void openCommand() {
        hasReachedCommandEnd = false;
        currentValid = true;
        if (current == ';' || current == '\n') {
            read();
        }
    }

    void closeCommand();

    bool isCommandOpen() const {
        return !hasReachedCommandEnd;
    }

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
};

#include "RCodeSequenceInStream.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODEINSTREAM_HPP_ */

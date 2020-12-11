/*
 * RCodeCommandInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_INSTREAMS_RCODECOMMANDINSTREAM_HPP_
#define SRC_MAIN_CPP_RCODE_INSTREAMS_RCODECOMMANDINSTREAM_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeLookaheadStream.hpp"

class RCodeSequenceInStream;
class RCodeLookaheadStream;
class RCodeCommandInStream;

class RCodeCommandLookaheadStream: public RCodeLookaheadStream {
private:
    RCodeLookaheadStream *parent;
    RCodeCommandInStream *commandIn;
    uint8_t lookahead = 2;
public:
    RCodeCommandLookaheadStream() :
            parent(NULL), commandIn(
            NULL) {
    }
    virtual char read();

    void reset(RCodeCommandInStream *commandIn);
};

class RCodeCommandInStream {
private:
    RCodeSequenceInStream *sequenceIn;
    RCodeCommandLookaheadStream lookahead;
    char current = 0;
    bool opened = false;
    bool inString = false;
    bool backslash = false;

    void readInternal();
    friend RCodeCommandLookaheadStream;
public:
    RCodeCommandInStream(RCodeSequenceInStream *sequenceIn) :
            sequenceIn(sequenceIn), lookahead() {
    }

    void reset() {
        current = 0;
        opened = false;
        inString = false;
        backslash = false;
    }

    void open() {
        if (!opened) {
            opened = true;
            readInternal();
        }
    }

    void openWith(char c) {
        opened = true;
        current = c;
    }

    void close() {
        while (opened) {
            readInternal();
        }
    }

    char read() {
        if (opened) {
            char prev = current;
            readInternal();
            return prev;
        } else {
            return current;
        }
    }

    char peek() {
        return current;
    }

    bool hasNext() {
        return opened;
    }

    void eatWhitespace() {
        while (opened && (current == ' ' || current == '\t' || current == '\r')) {
            readInternal();
        }
    }

    RCodeLookaheadStream* getLookahead() {
        lookahead.reset(this);
        return &lookahead;
    }
};
#include "RCodeSequenceInStream.hpp"

#endif /* SRC_MAIN_CPP_RCODE_INSTREAMS_RCODECOMMANDINSTREAM_HPP_ */

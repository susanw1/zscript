/*
 * ZcodeCommandInStream.hpp
 *
 *  Created on: 8 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECOMMANDINSTREAM_HPP_
#define SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECOMMANDINSTREAM_HPP_
#include "../ZcodeIncludes.hpp"
#include "ZcodeLookaheadStream.hpp"

template<class ZP>
class ZcodeSequenceInStream;

template<class ZP>
class ZcodeLookaheadStream;

template<class ZP>
class ZcodeCommandInStream;

template<class ZP>
class ZcodeCommandLookaheadStream: public ZcodeLookaheadStream<ZP> {
private:
    ZcodeLookaheadStream<ZP> *parent;
    ZcodeCommandInStream<ZP> *commandIn;
    uint8_t lookahead = 2;
public:
    ZcodeCommandLookaheadStream() :
            parent(NULL), commandIn(NULL) {
    }
    virtual char read() {
        if (lookahead == 0) {
            return parent->read();
        }
        if (lookahead-- == 2) {
            return commandIn->current;
        } else {
            return commandIn->sequenceIn->peek();
        }
    }

    void reset(ZcodeCommandInStream<ZP> *commandIn) {
        this->commandIn = commandIn;
        this->parent = commandIn->sequenceIn->getChannelInStream()->getLookahead();
        this->lookahead = 2;
    }
};

template<class ZP>
class ZcodeCommandInStream {
private:
    ZcodeSequenceInStream<ZP> *sequenceIn;
    ZcodeCommandLookaheadStream<ZP> lookahead;
    char current = 0;
    bool opened = false;
    bool inString = false;
    bool backslash = false;

    void readInternal();
    friend ZcodeCommandLookaheadStream<ZP> ;
public:
    ZcodeCommandInStream(ZcodeSequenceInStream<ZP> *sequenceIn) :
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

    ZcodeLookaheadStream<ZP>* getLookahead() {
        lookahead.reset(this);
        return &lookahead;
    }
};

template<class ZP>
void ZcodeCommandInStream<ZP>::readInternal() {
    current = sequenceIn->read();

    if (current == Zchars::EOL_SYMBOL || (!inString && (current == Zchars::ANDTHEN_SYMBOL || current == Zchars::ORELSE_SYMBOL))) {
        opened = false;
    } else if (current == Zchars::BIGFIELD_QUOTE_MARKER && !backslash) {
        inString = !inString;
    } else if (inString) {
        if (current == '\\') {
            backslash = !backslash;
        } else {
            backslash = false;
        }
    }
}

#include "ZcodeSequenceInStream.hpp"

#endif /* SRC_MAIN_CPP_ZCODE_INSTREAMS_ZCODECOMMANDINSTREAM_HPP_ */

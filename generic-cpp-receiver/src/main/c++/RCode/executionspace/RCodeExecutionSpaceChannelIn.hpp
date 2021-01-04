/*
 * RCodeExecutionSpaceSequenceIn.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACESEQUENCEIN_HPP_
#define SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACESEQUENCEIN_HPP_
#include "../instreams/RCodeChannelInStream.hpp"
#include "../RCodeIncludes.hpp"

template<class RP>
class RCodeExecutionSpace;

template<class RP>
class RCodeExecutionSpaceChannelIn;

template<class RP>
class RCodeExecutionSpaceLookahead: public RCodeLookaheadStream<RP> {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
private:
    RCodeExecutionSpaceChannelIn<RP> *parent = NULL;
    executionSpaceAddress_t offset = 0;
public:
    RCodeExecutionSpaceLookahead(RCodeExecutionSpaceChannelIn<RP> *parent) :
            parent(parent) {
    }

    void reset() {
        offset = 0;
    }

    char read() {
        int16_t ch = parent->getAtOffset(offset);
        return (ch != -1)? (offset++, (char)ch) : '\n';
    }
};

template<class RP>
class RCodeExecutionSpaceChannelIn: public RCodeChannelInStream<RP> {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
private:
    RCodeExecutionSpaceLookahead<RP> lookahead;
    RCodeExecutionSpace<RP> *space;
    executionSpaceAddress_t pos = 0;
    bool inUse = false;

public:
    RCodeExecutionSpaceChannelIn() :
            lookahead(this), space(NULL) {
    }
    void initialSetup(RCodeExecutionSpace<RP> *space) {
        this->space = space;
    }

    void setup(executionSpaceAddress_t position) {
        pos = position;
        inUse = true;
    }

    void release() {
        inUse = false;
    }

    bool isInUse() {
        return inUse;
    }

    int16_t read() {
        int16_t ch = getAtOffset(0);
        if (ch != -1) {
            pos++;
        }
        return ch;
    }

    int16_t getAtOffset(executionSpaceAddress_t offset) {
        if (pos + offset >= space->getLength() || space->get((executionSpaceAddress_t)(pos + offset)) == '\n') {
            return -1;
        } else {
            return (char) space->get(pos);
        }
    }

    virtual RCodeLookaheadStream<RP>* getLookahead() {
        lookahead.reset();
        return &lookahead;
    }

    executionSpaceAddress_t getPosition() {
        return pos;
    }

    bool needsDelayNext() {
        return pos >= space->getLength() || (pos == space->getLength() - 1 && space->get(pos) == '\n');
    }
};

#include "RCodeExecutionSpace.hpp"

#endif /* SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACESEQUENCEIN_HPP_ */

/*
 * ZcodeExecutionSpaceSequenceIn.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACESEQUENCEIN_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACESEQUENCEIN_HPP_
#include "../instreams/ZcodeChannelInStream.hpp"
#include "../ZcodeIncludes.hpp"

template<class RP>
class ZcodeExecutionSpace;

template<class RP>
class ZcodeExecutionSpaceChannelIn;

template<class RP>
class ZcodeExecutionSpaceLookahead: public ZcodeLookaheadStream<RP> {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
private:
    ZcodeExecutionSpaceChannelIn<RP> *parent = NULL;
    executionSpaceAddress_t offset = 0;
public:
    ZcodeExecutionSpaceLookahead(ZcodeExecutionSpaceChannelIn<RP> *parent) :
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
class ZcodeExecutionSpaceChannelIn: public ZcodeChannelInStream<RP> {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
private:
    ZcodeExecutionSpaceLookahead<RP> lookahead;
    ZcodeExecutionSpace<RP> *space;
    executionSpaceAddress_t pos = 0;
    bool inUse = false;

public:
    ZcodeExecutionSpaceChannelIn() :
            lookahead(this), space(NULL) {
    }
    void initialSetup(ZcodeExecutionSpace<RP> *space) {
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

    virtual ZcodeLookaheadStream<RP>* getLookahead() {
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

#include "ZcodeExecutionSpace.hpp"

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACESEQUENCEIN_HPP_ */

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
#include "RCodeParameters.hpp"

class RCodeExecutionSpace;

class RCodeExecutionSpaceChannelIn: public RCodeChannelInStream {
private:
    class RCodeExecutionSpaceLookahead: public RCodeLookaheadStream {
    private:
        RCodeExecutionSpaceChannelIn *parent = NULL;
        executionSpaceAddress_t offset = 0;
        bool stillInSequence = false;
    public:
        RCodeExecutionSpaceLookahead(RCodeExecutionSpaceChannelIn *parent) :
                parent(parent) {

        }
        void reset();
        char read();
    };
    RCodeExecutionSpaceLookahead lookahead;
    RCodeExecutionSpace *space;
    executionSpaceAddress_t pos = 0;
    bool inUse = false;

public:
    RCodeExecutionSpaceChannelIn() :
            lookahead(this), space(NULL) {
    }
    void initialSetup(RCodeExecutionSpace *space) {
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

    int16_t read();

    virtual RCodeLookaheadStream* getLookahead() {
        lookahead.reset();
        return &lookahead;
    }

    int getPosition() {
        return pos;
    }

    char next();

    bool needsDelayNext();
};
#include "RCodeExecutionSpace.hpp"

#endif /* SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACESEQUENCEIN_HPP_ */

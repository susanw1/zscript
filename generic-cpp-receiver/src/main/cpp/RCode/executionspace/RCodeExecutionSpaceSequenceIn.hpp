/*
 * RCodeExecutionSpaceSequenceIn.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACESEQUENCEIN_HPP_
#define SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACESEQUENCEIN_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "../parsing/RCodeSequenceInStream.hpp"

class RCodeExecutionSpace;

class RCodeExecutionSpaceSequenceIn: public RCodeSequenceInStream {
private:
    class RCodeExecutionSpaceLookahead: public RCodeLookaheadStream {
    private:
        RCodeExecutionSpaceSequenceIn *parent = NULL;
        executionSpaceAddress_t offset = 0;
        bool stillInSequence = false;
    public:
        RCodeExecutionSpaceLookahead(RCodeExecutionSpaceSequenceIn *parent) :
                parent(parent) {

        }
        void reset();
        char read();
    };
    RCodeExecutionSpaceLookahead lookahead;
    RCodeExecutionSpace *space;
    executionSpaceAddress_t pos = 0;
    bool isSequenceOpen = false;
    bool hasStarted = false;
    bool inUse = false;

public:
    RCodeExecutionSpaceSequenceIn() :
            lookahead(this), space(NULL) {
    }
    void initialSetup(RCodeExecutionSpace *space) {
        this->space = space;
    }

    void setup(executionSpaceAddress_t position) {
        pos = position;
        isSequenceOpen = false;
        hasStarted = false;
        inUse = true;
    }

    void release() {
        inUse = false;
    }

    bool isInUse() {
        return inUse;
    }

    bool hasNext() {
        return isSequenceOpen || !hasStarted;
    }

    RCodeLookaheadStream* getLookahead() {
        lookahead.reset();
        return &lookahead;
    }

    int getPosition() {
        return pos;
    }

    bool isCommandSequenceOpen() const {
        return isSequenceOpen;
    }

    char next();

    void openCommandSequence();

    bool needsDelayNext();

    void closeCommandSequence();
};
#include "RCodeExecutionSpace.hpp"

#endif /* SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACESEQUENCEIN_HPP_ */

/*
 * RCodeExecutionSpaceOut.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACEOUT_HPP_
#define SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACEOUT_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "../AbstractRCodeOutStream.hpp"

class RCodeExecutionSpace;
class RCodeExecutionSpaceOut: public AbstractRCodeOutStream {
private:
    RCodeExecutionSpace *space;
    uint8_t buffer[RCodeParameters::executionOutBufferSize];
    executionSpaceOutLength_t length = 0;
    int lastEndPos = 0;
    bool overLength = false;
    bool inUse = false;
    bool dataBufferFull = false;
    bool open = false;
    RCodeResponseStatus status = OK;

public:
    RCodeExecutionSpaceOut() :
            space(NULL) {
    }
    void initialSetup(RCodeExecutionSpace *space) {
        this->space = space;
    }
    void writeByte(uint8_t value) {
        if (length == RCodeParameters::executionOutBufferSize) {
            overLength = true;
        }
        if (open && !overLength) {
            buffer[length++] = value;
        }
    }

    virtual RCodeOutStream* writeBytes(uint8_t const *value, uint16_t l) {
        if (open) {
            if (length == RCodeParameters::executionOutBufferSize) {
                overLength = true;
            }
            for (int i = 0; i < l && !overLength; i++) {
                buffer[length++] = value[i];
                if (length == RCodeParameters::executionOutBufferSize) {
                    overLength = true;
                }
            }
        }
        return this;
    }

    RCodeOutStream* writeCommandSeperator() {
        if (!overLength) {
            lastEndPos = length;
        }
        return AbstractRCodeOutStream::writeCommandSeperator();
    }
    RCodeOutStream* writeCommandSequenceErrorHandler() {
        if (!overLength) {
            lastEndPos = length;
        }
        return AbstractRCodeOutStream::writeCommandSequenceErrorHandler();
    }

    RCodeOutStream* writeStatus(RCodeResponseStatus st) {
        if (st != OK) {
            status = st;
        }
        return AbstractRCodeOutStream::writeStatus(st);
    }

    void openResponse(RCodeCommandChannel *target) {
        open = true;
        length = 0;
        overLength = false;
    }

    void openNotification(RCodeCommandChannel *target) {
    }

    void openDebug(RCodeCommandChannel *target) {
    }

    bool isOpen() {
        return open;
    }

    void close() {
        open = false;
        if (status != OK) {
            flush();
        } else {
            inUse = false;
        }
    }

    bool flush();

    bool isDataBufferFull() {
        return dataBufferFull;
    }

    void setInUse(bool inUse) {
        this->inUse = inUse;
    }

    bool isInUse() {
        return inUse;
    }
};

#include "../parsing/RCodeCommandChannel.hpp"
#include "RCodeExecutionSpace.hpp"
#endif /* SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACEOUT_HPP_ */

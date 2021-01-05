/*
 * RCodeExecutionSpaceOut.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACEOUT_HPP_
#define SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACEOUT_HPP_
#include "../RCodeIncludes.hpp"
#include "../AbstractRCodeOutStream.hpp"

template<class RP>
class RCodeExecutionSpace;

template<class RP>
class RCodeExecutionSpaceOut: public AbstractRCodeOutStream<RP> {
    typedef typename RP::executionSpaceOutLength_t executionSpaceOutLength_t;
    private:
    RCodeExecutionSpace<RP> *space;
    uint8_t buffer[RP::executionOutBufferSize];
    executionSpaceOutLength_t length = 0;
    executionSpaceOutLength_t lastEndPos = 0;
    bool overLength = false;
    bool inUse = false;
    bool dataBufferFull = false;
    bool open = false;
    RCodeResponseStatus status = OK;

public:
    RCodeExecutionSpaceOut() :
            space(NULL) {
    }
    void initialSetup(RCodeExecutionSpace<RP> *space) {
        this->space = space;
    }

    void writeByte(uint8_t value) {
        if (length == RP::executionOutBufferSize) {
            overLength = true;
        }
        if (open && !overLength) {
            buffer[length++] = value;
        }
    }

    virtual RCodeOutStream<RP>* writeBytes(uint8_t const *value, uint16_t l) {
        if (open) {
            if (length == RP::executionOutBufferSize) {
                overLength = true;
            }
            for (int i = 0; i < l && !overLength; i++) {
                buffer[length++] = value[i];
                if (length == RP::executionOutBufferSize) {
                    overLength = true;
                }
            }
        }
        return this;
    }

    RCodeOutStream<RP>* writeCommandSeperator() {
        if (!overLength) {
            lastEndPos = length;
        }
        return AbstractRCodeOutStream<RP>::writeCommandSeperator();
    }

    RCodeOutStream<RP>* writeCommandSequenceErrorHandler() {
        if (!overLength) {
            lastEndPos = length;
        }
        return AbstractRCodeOutStream<RP>::writeCommandSequenceErrorHandler();
    }

    RCodeOutStream<RP>* writeStatus(RCodeResponseStatus st) {
        if (st != OK) {
            status = st;
        }
        return AbstractRCodeOutStream<RP>::writeStatus(st);
    }

    void openResponse(RCodeCommandChannel<RP> *target) {
        open = true;
        length = 0;
        overLength = false;
    }

    void openNotification(RCodeCommandChannel<RP> *target) {
    }

    void openDebug(RCodeCommandChannel<RP> *target) {
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

template<class RP>
bool RCodeExecutionSpaceOut<RP>::flush() {
    if (status != CMD_FAIL && status != OK) {
        space->setRunning(false);
        space->setFailed(true);
    }
    if (!space->getNotificationChannel()->hasOutStream() || !space->getNotificationChannel()->acquireOutStream()->isLocked()) {
        RCodeOutStream<RP> *out = space->getNotificationChannel()->acquireOutStream();
        out->lock();
        if (out->isOpen()) {
            out->close();
        }
        out->openNotification(space->getNotificationChannel());
        out->mostRecent = space;
        out->markNotification();
        out->writeField('Z', (uint8_t) 2);
        if (overLength) {
            out->writeBytes(buffer, lastEndPos);
            out->writeCommandSeperator();
            out->writeStatus(RESP_TOO_LONG);
            out->writeCommandSeperator();
            out->writeStatus(status);
            out->writeCommandSequenceSeperator();
        } else {
            out->writeBytes(buffer, length);
        }
        out->close();
        out->unlock();
        space->getNotificationChannel()->releaseOutStream();
        dataBufferFull = false;
        status = OK;
        return true;
    } else {
        dataBufferFull = true;
        status = OK;
        return false;
    }
}

#endif /* SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACEOUT_HPP_ */

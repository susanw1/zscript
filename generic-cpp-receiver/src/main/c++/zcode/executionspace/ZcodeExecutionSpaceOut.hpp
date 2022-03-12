/*
 * ZcodeExecutionSpaceOut.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACEOUT_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACEOUT_HPP_
#include "../ZcodeIncludes.hpp"
#include "../AbstractZcodeOutStream.hpp"

template<class RP>
class ZcodeExecutionSpace;

template<class RP>
class ZcodeExecutionSpaceOut: public AbstractZcodeOutStream<RP> {
    typedef typename RP::executionSpaceOutLength_t executionSpaceOutLength_t;
    private:
    ZcodeExecutionSpace<RP> *space;
    uint8_t buffer[RP::executionOutBufferSize];
    executionSpaceOutLength_t length = 0;
    executionSpaceOutLength_t lastEndPos = 0;
    bool overLength = false;
    bool inUse = false;
    bool dataBufferFull = false;
    bool open = false;
    ZcodeResponseStatus status = OK;

public:
    ZcodeExecutionSpaceOut() :
            space(NULL) {
    }
    void initialSetup(ZcodeExecutionSpace<RP> *space) {
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

    virtual ZcodeOutStream<RP>* writeBytes(uint8_t const *value, uint16_t l) {
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

    ZcodeOutStream<RP>* writeCommandSeperator() {
        if (!overLength) {
            lastEndPos = length;
        }
        return AbstractZcodeOutStream<RP>::writeCommandSeperator();
    }

    ZcodeOutStream<RP>* writeCommandSequenceErrorHandler() {
        if (!overLength) {
            lastEndPos = length;
        }
        return AbstractZcodeOutStream<RP>::writeCommandSequenceErrorHandler();
    }

    ZcodeOutStream<RP>* writeStatus(ZcodeResponseStatus st) {
        if (st != OK) {
            status = st;
        }
        return AbstractZcodeOutStream<RP>::writeStatus(st);
    }

    void openResponse(ZcodeCommandChannel<RP> *target) {
        open = true;
        length = 0;
        overLength = false;
    }

    void openNotification(ZcodeCommandChannel<RP> *target) {
    }

    void openDebug(ZcodeCommandChannel<RP> *target) {
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

#include "../parsing/ZcodeCommandChannel.hpp"
#include "ZcodeExecutionSpace.hpp"

template<class RP>
bool ZcodeExecutionSpaceOut<RP>::flush() {
    if (status != CMD_FAIL && status != OK) {
        space->setRunning(false);
        space->setFailed(true);
    }
    if (!space->getNotificationChannel()->hasOutStream() || !space->getNotificationChannel()->acquireOutStream()->isLocked()) {
        ZcodeOutStream<RP> *out = space->getNotificationChannel()->acquireOutStream();
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

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACEOUT_HPP_ */

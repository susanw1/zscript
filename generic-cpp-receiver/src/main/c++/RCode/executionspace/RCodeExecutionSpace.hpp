/*
 * RCodeExecutionSpace.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACE_HPP_
#define SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACE_HPP_
#include "../RCodeIncludes.hpp"

#include "RCodeExecutionSpaceChannelIn.hpp"
#include "RCodeExecutionSpaceOut.hpp"

template<class RP>
class RCodeNotificationManager;

template<class RP>
class RCodeCommandChannel;

template<class RP>
class RCodeExecutionSpaceChannel;

template<class RP>
class RCodeExecutionSpace {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
public:
    RCodeExecutionSpace();
private:
    RCodeExecutionSpaceChannelIn<RP> inStreams[RP::executionInNum];
    RCodeExecutionSpaceOut<RP> outStreams[RP::executionOutNum];
    RCodeNotificationManager<RP> *notifications;
    RCodeExecutionSpaceChannel<RP> **channels = NULL;
    uint8_t channelNum = 0;
    executionSpaceAddress_t length = 0;
    uint8_t delay = 0;
    uint8_t space[RP::executionLength];
    bool running = false;
    bool failed = false;

public:
    RCodeExecutionSpace(RCodeNotificationManager<RP> *notifications) :
            notifications(notifications) {
        for (int i = 0; i < RP::executionInNum; ++i) {
            inStreams[i].initialSetup(this);
        }
        for (int i = 0; i < RP::executionOutNum; ++i) {
            outStreams[i].initialSetup(this);
        }
        for (int i = 0; i < RP::executionLength; ++i) {
            space[i] = 0;
        }
    }
    void setChannels(RCodeExecutionSpaceChannel<RP> **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }

    uint8_t getChannelNum() {
        return channelNum;
    }

    RCodeExecutionSpaceChannel<RP>** getChannels() {
        return channels;
    }

    uint8_t get(executionSpaceAddress_t pos) {
        return space[pos];
    }

    executionSpaceAddress_t getLength() {
        return length;
    }

    bool hasInStream() {
        for (int i = 0; i < RP::executionInNum; i++) {
            if (!inStreams[i].isInUse()) {
                return true;
            }
        }
        return false;
    }

    RCodeExecutionSpaceChannelIn<RP>* acquireInStream(executionSpaceAddress_t position) {
        for (int i = 0; i < RP::executionInNum; i++) {
            if (!inStreams[i].isInUse()) {
                inStreams[i].setup(position);
                return inStreams + i;
            }
        }
        return NULL;
    }

    void releaseInStream(RCodeExecutionSpaceChannelIn<RP> *stream) {
        stream->release();
    }

    bool hasOutStream() {
        for (int i = 0; i < RP::executionOutNum; i++) {
            if (!outStreams[i].isInUse()) {
                return true;
            }
        }
        return false;
    }

    RCodeExecutionSpaceOut<RP>* acquireOutStream() {
        for (int i = 0; i < RP::executionOutNum; i++) {
            if (!outStreams[i].isInUse()) {
                outStreams[i].setInUse(true);
                return outStreams + i;
            }
        }
        return NULL;
    }

    void releaseOutStream(RCodeExecutionSpaceOut<RP> *stream) {
        if (!stream->isDataBufferFull()) {
            stream->setInUse(false);
        }
    }

    void flush() {
        for (int i = 0; i < RP::executionOutNum; i++) {
            if (outStreams[i].isDataBufferFull() && outStreams[i].flush()) {
                outStreams[i].setInUse(false);
            }
        }
    }

    RCodeCommandChannel<RP>* getNotificationChannel() {
        return notifications->getNotificationChannel();
    }

    void setDelay(uint8_t delay) {
        this->delay = delay;
    }

    uint8_t getDelay() {
        return delay;
    }

    bool isRunning() {
        return running;
    }

    void setRunning(bool b) {
        running = b;
    }
    void setFailed(bool b) {
        failed = b;
    }
    bool hasFailed() {
        return failed;
    }

    void write(const uint8_t *data, executionSpaceAddress_t length, executionSpaceAddress_t address, bool isEnd) {
        for (int i = 0; i < length; i++) {
            space[i + address] = data[i];
        }
        if (isEnd) {
            this->length = (executionSpaceAddress_t)(address + length);
        }
    }
};

#include "../RCodeNotificationManager.hpp"
#endif /* SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACE_HPP_ */

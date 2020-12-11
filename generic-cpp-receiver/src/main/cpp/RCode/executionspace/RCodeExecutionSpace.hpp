/*
 * RCodeExecutionSpace.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACE_HPP_
#define SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACE_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeExecutionSpaceChannelIn.hpp"
#include "RCodeExecutionSpaceOut.hpp"

class RCodeNotificationManager;
class RCodeCommandChannel;

class RCodeExecutionSpaceChannel;

class RCodeExecutionSpace {
public:
    RCodeExecutionSpace();
private:
    RCodeExecutionSpaceChannelIn inStreams[RCodeParameters::executionInNum];
    RCodeExecutionSpaceOut outStreams[RCodeParameters::executionOutNum];
    RCodeNotificationManager *notifications;
    RCodeExecutionSpaceChannel **channels = NULL;
    uint8_t channelNum = 0;
    executionSpaceAddress_t length = 0;
    uint8_t delay = 0;
    uint8_t space[RCodeParameters::executionLength];
    bool running = false;
    bool failed = false;

public:
    RCodeExecutionSpace(RCodeNotificationManager *notifications) :
            notifications(notifications) {
        for (int i = 0; i < RCodeParameters::executionInNum; ++i) {
            inStreams[i].initialSetup(this);
        }
        for (int i = 0; i < RCodeParameters::executionOutNum; ++i) {
            outStreams[i].initialSetup(this);
        }
        for (int i = 0; i < RCodeParameters::executionLength; ++i) {
            space[i] = 0;
        }
    }
    void setChannels(RCodeExecutionSpaceChannel **channels,
            uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }

    uint8_t getChannelNum() {
        return channelNum;
    }

    RCodeExecutionSpaceChannel** getChannels() {
        return channels;
    }

    uint8_t get(executionSpaceAddress_t pos) {
        return space[pos];
    }

    executionSpaceAddress_t getLength() {
        return length;
    }

    bool hasInStream() {
        for (int i = 0; i < RCodeParameters::executionInNum; i++) {
            if (!inStreams[i].isInUse()) {
                return true;
            }
        }
        return false;
    }

    RCodeExecutionSpaceChannelIn* acquireInStream(
            executionSpaceAddress_t position) {
        for (int i = 0; i < RCodeParameters::executionInNum; i++) {
            if (!inStreams[i].isInUse()) {
                inStreams[i].setup(position);
                return inStreams + i;
            }
        }
        return NULL;
    }

    void releaseInStream(RCodeExecutionSpaceChannelIn *stream) {
        stream->release();
    }

    bool hasOutStream() {
        for (int i = 0; i < RCodeParameters::executionOutNum; i++) {
            if (!outStreams[i].isInUse()) {
                return true;
            }
        }
        return false;
    }

    RCodeExecutionSpaceOut* acquireOutStream() {
        for (int i = 0; i < RCodeParameters::executionOutNum; i++) {
            if (!outStreams[i].isInUse()) {
                outStreams[i].setInUse(true);
                return outStreams + i;
            }
        }
        return NULL;
    }

    void releaseOutStream(RCodeExecutionSpaceOut *stream) {
        if (!stream->isDataBufferFull()) {
            stream->setInUse(false);
        }
    }

    void flush() {
        for (int i = 0; i < RCodeParameters::executionOutNum; i++) {
            if (outStreams[i].isDataBufferFull() && outStreams[i].flush()) {
                outStreams[i].setInUse(false);
            }
        }
    }

    RCodeCommandChannel* getNotificationChannel();

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

    void write(const uint8_t *data, executionSpaceAddress_t length,
            executionSpaceAddress_t address, bool isEnd) {
        for (int i = 0; i < length; i++) {
            space[i + address] = data[i];
        }
        if (isEnd) {
            this->length = address + length;
        }
    }
};
#include "../RCodeNotificationManager.hpp"
#endif /* SRC_TEST_CPP_RCODE_EXECUTIONSPACE_RCODEEXECUTIONSPACE_HPP_ */

/*
 * ZcodeExecutionSpace.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_
#include "../ZcodeIncludes.hpp"

#include "ZcodeExecutionSpaceChannelIn.hpp"
#include "ZcodeExecutionSpaceOut.hpp"

template<class ZP>
class ZcodeNotificationManager;

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeExecutionSpaceChannel;

template<class ZP>
class ZcodeExecutionSpace {
    public:
        ZcodeExecutionSpace();

    private:
        typedef typename ZP::executionSpaceAddress_t executionSpaceAddress_t;

        ZcodeExecutionSpaceChannelIn<ZP> inStreams[ZP::executionInNum];
        ZcodeExecutionSpaceOut<ZP> outStreams[ZP::executionOutNum];
        ZcodeNotificationManager<ZP> *notifications;
        ZcodeExecutionSpaceChannel<ZP> **channels = NULL;
        uint8_t channelNum = 0;
        executionSpaceAddress_t length = 0;
        uint8_t delay = 0;
        uint8_t space[ZP::executionLength];
        bool running = false;
        bool failed = false;

    public:
        ZcodeExecutionSpace(ZcodeNotificationManager<ZP> *notifications) :
                notifications(notifications) {
            for (int i = 0; i < ZP::executionInNum; ++i) {
                inStreams[i].initialSetup(this);
            }
            for (int i = 0; i < ZP::executionOutNum; ++i) {
                outStreams[i].initialSetup(this);
            }
            for (int i = 0; i < ZP::executionLength; ++i) {
                space[i] = 0;
            }
        }
        void setChannels(ZcodeExecutionSpaceChannel<ZP> **channels, uint8_t channelNum) {
            this->channels = channels;
            this->channelNum = channelNum;
        }

        uint8_t getChannelNum() {
            return channelNum;
        }

        ZcodeExecutionSpaceChannel<ZP>** getChannels() {
            return channels;
        }

        uint8_t get(executionSpaceAddress_t pos) {
            return space[pos];
        }

        executionSpaceAddress_t getLength() {
            return length;
        }

        bool hasInStream() {
            for (int i = 0; i < ZP::executionInNum; i++) {
                if (!inStreams[i].isInUse()) {
                    return true;
                }
            }
            return false;
        }

        ZcodeExecutionSpaceChannelIn<ZP>* acquireInStream(executionSpaceAddress_t position) {
            for (int i = 0; i < ZP::executionInNum; i++) {
                if (!inStreams[i].isInUse()) {
                    inStreams[i].setup(position);
                    return inStreams + i;
                }
            }
            return NULL;
        }

        void releaseInStream(ZcodeExecutionSpaceChannelIn<ZP> *stream) {
            stream->release();
        }

        bool hasOutStream() {
            for (int i = 0; i < ZP::executionOutNum; i++) {
                if (!outStreams[i].isInUse()) {
                    return true;
                }
            }
            return false;
        }

        ZcodeExecutionSpaceOut<ZP>* acquireOutStream() {
            for (int i = 0; i < ZP::executionOutNum; i++) {
                if (!outStreams[i].isInUse()) {
                    outStreams[i].setInUse(true);
                    return outStreams + i;
                }
            }
            return NULL;
        }

        void releaseOutStream(ZcodeExecutionSpaceOut<ZP> *stream) {
            if (!stream->isDataBufferFull()) {
                stream->setInUse(false);
            }
        }

        void flush() {
            for (int i = 0; i < ZP::executionOutNum; i++) {
                if (outStreams[i].isDataBufferFull() && outStreams[i].flush()) {
                    outStreams[i].setInUse(false);
                }
            }
        }

        ZcodeCommandChannel<ZP>* getNotificationChannel() {
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
                this->length = (executionSpaceAddress_t) (address + length);
            }
        }
};

#include "../ZcodeNotificationManager.hpp"
#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_ */

/*
 * ZcodeExecutionSpace.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeScriptSpaceChannel.hpp"

template<class ZP>
class ZcodeNotificationManager;

template<class ZP>
class ZcodeScriptSpace {
public:
    ZcodeScriptSpace();

private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;

    ZcodeNotificationManager<ZP> *notifications;
    ZcodeScriptSpaceChannel<ZP> **channels = NULL;
    uint8_t channelNum = 0;
    uint8_t space[ZP::scriptLength];
    scriptSpaceAddress_t length = 0;
    uint8_t delay = 0;
    bool running = false;
    bool failed = false;

public:
    ZcodeScriptSpace(ZcodeNotificationManager<ZP> *notifications) :
            notifications(notifications) {
        for (scriptSpaceAddress_t i = 0; i < ZP::scriptLength; ++i) {
            space[i] = 0;
        }
    }
    void setChannels(ZcodeScriptSpaceChannel<ZP> **channels, uint8_t channelNum) {
        this->channels = channels;
        this->channelNum = channelNum;
    }

    uint8_t getChannelNum() {
        return channelNum;
    }

    ZcodeScriptSpaceChannel<ZP>** getChannels() {
        return channels;
    }

    uint8_t get(scriptSpaceAddress_t pos) {
        return space[pos];
    }

    scriptSpaceAddress_t getLength() {
        return length;
    }

    void flush() {
        for (scriptSpaceAddress_t i = 0; i < channelNum; i++) {
            if (channels[i]->getOutStream()->isDataBufferFull()) {
                channels[i]->getOutStream()->flush();
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

    void write(const uint8_t *data, scriptSpaceAddress_t length, scriptSpaceAddress_t address, bool isEnd) {
        for (scriptSpaceAddress_t i = 0; i < length; i++) {
            space[i + address] = data[i];
        }
        if (isEnd) {
            this->length = (scriptSpaceAddress_t) (address + length);
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_ */

/*
 * ZcodeExecutionSpace.hpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_
#define SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#include <string.h>

#include "../ZcodeIncludes.hpp"
#include "ZcodeScriptSpaceChannel.hpp"

template<class ZP>
class ZcodeNotificationManager;

//Solving C++'s problems with initialising an array of object values...
//template<class ZP>
//union ZcodeEvilArrayInitialiserSolution {
//    ZcodeScriptSpaceChannel<ZP> channel;
//    uint8_t b;
//    ZcodeEvilArrayInitialiserSolution() :
//            b(0) {
//    }
//    ZcodeEvilArrayInitialiserSolution() :
//            channel() {
//    }
//};

template<class ZP>
class ZcodeScriptSpace {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;

    ZcodeNotificationManager<ZP> *notifications;
    //ZcodeEvilArrayInitialiserSolution<ZP> channelActual;
    ZcodeScriptSpaceChannel<ZP> channels[ZP::scriptChannelCount];
    uint8_t space[ZP::scriptLength];
    scriptSpaceAddress_t length = 0;
    uint8_t delay = 0;
    bool running = false;
    bool failed = false;

public:
    ZcodeScriptSpace() :
            notifications(Zcode<ZP>::zcode.getNotificationManager()) {
        for (scriptSpaceAddress_t i = 0; i < ZP::scriptLength; ++i) {
            space[i] = 0;
        }
//        for (uint8_t i = 0; i < ZP::scriptChannelCount - 1; ++i) {
//            memcpy((void*) (channels + i), (void*) (&channelActual), sizeof(ZcodeEvilArrayInitialiserSolution<ZP> ));
//        }
    }


    uint8_t getChannelCount() {
        return ZP::scriptChannelCount;
    }

    ZcodeScriptSpaceChannel<ZP>* getChannel(uint8_t i) {
            return &channels[i];

    }

    uint8_t get(scriptSpaceAddress_t pos) {
        return space[pos];
    }

    scriptSpaceAddress_t getLength() {
        return length;
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
#endif
#endif /* SRC_TEST_CPP_ZCODE_EXECUTIONSPACE_ZCODEEXECUTIONSPACE_HPP_ */

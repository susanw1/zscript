/*
 * ZcodeInterruptVectorManager.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMANAGER_HPP_
#define SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMANAGER_HPP_

#include "../scriptspace/ZcodeInterruptVectorMap.hpp"
#include "../scriptspace/ZcodeInterruptVectorOut.hpp"
#include "../scriptspace/ZcodeScriptSpace.hpp"
#include "../ZcodeIncludes.hpp"
#include "../ZcodeBusInterrupt.hpp"

template<class ZP>
class ZcodeInterruptVectorManager {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;

    ZcodeNotificationManager<ZP> *notificationManager;
    ZcodeScriptSpace<ZP> *space;
    ZcodeInterruptVectorOut<ZP> out;
    ZcodeInterruptVectorChannel<ZP> channel;
    ZcodeInterruptVectorMap<ZP> vectorMap;
    ZcodeBusInterrupt<ZP> waitingInterrupts[ZP::interruptVectorWorkingNum];
    uint8_t waitingNum = 0;

public:
    ZcodeInterruptVectorManager(ZcodeNotificationManager<ZP> *notificationManager, ZcodeScriptSpace<ZP> *space) :
            notificationManager(notificationManager), space(space), channel(space, this, notificationManager) {
    }

    ZcodeInterruptVectorMap<ZP>* getVectorMap() {
        return &vectorMap;
    }

    bool canAccept() {
        return waitingNum < ZP::interruptVectorWorkingNum;
    }

    void acceptInterrupt(ZcodeBusInterrupt<ZP> i) {
        waitingInterrupts[waitingNum++] = i;
    }

    bool hasInterruptSource() {
        return waitingNum > 0;
    }

    void activateInterrupt() {
        if (!channel.hasInterrupt() && waitingNum > 0) {
            ZcodeBusInterrupt<ZP> interrupt = waitingInterrupts[0];
            for (uint8_t i = 0; i < waitingNum - 1; i++) {
                waitingInterrupts[i] = waitingInterrupts[i + 1];
            }
            waitingNum--;
            channel.setInterrupt(interrupt);
        }
    }

    ZcodeScriptSpace<ZP> getSpace() {
        return space;
    }

    bool hasVector(ZcodeBusInterrupt<ZP> *busInt) {
        return vectorMap.hasVector(busInt->getNotificationType(),
                busInt->getNotificationBus(),
                busInt->getFoundAddress(),
                busInt->hasFindableAddress());
    }

    scriptSpaceAddress_t findVector(ZcodeBusInterrupt<ZP> *busInt) {
        return vectorMap.getVector(busInt->getNotificationType(),
                busInt->getNotificationBus(),
                busInt->getFoundAddress(),
                busInt->hasFindableAddress());
    }

    ZcodeInterruptVectorOut<ZP>* getOut() {
        return &out;
    }

    ZcodeNotificationManager<ZP>* getNotificationManager() {
        return notificationManager;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMANAGER_HPP_ */

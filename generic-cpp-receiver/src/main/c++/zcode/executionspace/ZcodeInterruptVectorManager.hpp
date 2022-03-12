/*
 * ZcodeInterruptVectorManager.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMANAGER_HPP_
#define SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMANAGER_HPP_
#include "../ZcodeIncludes.hpp"
#include "../ZcodeBusInterrupt.hpp"
#include "ZcodeInterruptVectorOut.hpp"
#include "ZcodeInterruptVectorMap.hpp"
#include "ZcodeExecutionSpace.hpp"

template<class RP>
class ZcodeInterruptVectorManager {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
    private:
    ZcodeNotificationManager<RP> *notificationManager;
    ZcodeExecutionSpace<RP> space;
    ZcodeInterruptVectorOut<RP> out;
    ZcodeInterruptVectorMap<RP> vectorMap;
    ZcodeBusInterrupt<RP> waitingInterrupts[RP::interruptVectorWorkingNum];
    int waitingNum = 0;

public:
    ZcodeInterruptVectorManager(ZcodeNotificationManager<RP> *notificationManager, ZcodeExecutionSpace<RP> space) :
            notificationManager(notificationManager), space(space), out(notificationManager) {
    }

    ZcodeInterruptVectorMap<RP>* getVectorMap() {
        return &vectorMap;
    }

    bool canAccept() {
        return waitingNum < RP::interruptVectorWorkingNum;
    }

    void acceptInterrupt(ZcodeBusInterrupt<RP> i) {
        waitingInterrupts[waitingNum++] = i;
    }

    bool hasInterruptSource() {
        return waitingNum > 0;
    }

    ZcodeBusInterrupt<RP> takeInterrupt() {
        ZcodeBusInterrupt<RP> interrupt = waitingInterrupts[0];
        for (int i = 0; i < waitingNum - 1; i++) {
            waitingInterrupts[i] = waitingInterrupts[i + 1];
        }
        waitingNum--;
        return interrupt;
    }

    ZcodeExecutionSpace<RP> getSpace() {
        return space;
    }

    bool hasVector(ZcodeBusInterrupt<RP> *busInt) {
        return vectorMap.hasVector(busInt->getNotificationType(), busInt->getNotificationBus(), busInt->getFoundAddress(),
                busInt->getSource()->hasFindableAddress(busInt->getId()) && busInt->hasFindableAddress());

    }

    executionSpaceAddress_t findVector(ZcodeBusInterrupt<RP> *busInt) {
        return vectorMap.getVector(busInt->getNotificationType(), busInt->getNotificationBus(), busInt->getFoundAddress(),
                busInt->getSource()->hasAddress() && busInt->hasFindableAddress());
    }

    ZcodeInterruptVectorOut<RP>* getOut() {
        return &out;
    }

    ZcodeNotificationManager<RP>* getNotificationManager() {
        return notificationManager;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMANAGER_HPP_ */

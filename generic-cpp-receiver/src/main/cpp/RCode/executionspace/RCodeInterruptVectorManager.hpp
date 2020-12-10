/*
 * RCodeInterruptVectorManager.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORMANAGER_HPP_
#define SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORMANAGER_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeParameters.hpp"
#include "../RCodeBusInterrupt.hpp"
#include "RCodeInterruptVectorOut.hpp"
#include "RCodeInterruptVectorMap.hpp"

class RCodeInterruptVectorManager {
private:
    RCodeNotificationManager *notificationManager;
    RCodeExecutionSpace space;
    RCodeInterruptVectorOut out;
    RCodeInterruptVectorMap vectorMap;
    RCodeBusInterrupt *waitingInterrupts[RCodeParameters::interruptVectorWorkingNum];
    int waitingNum = 0;

public:
    RCodeInterruptVectorManager(RCodeNotificationManager *notificationManager,
            RCodeExecutionSpace space) :
            notificationManager(notificationManager), space(space), out(
                    notificationManager) {
    }

    RCodeInterruptVectorMap* getVectorMap() {
        return &vectorMap;
    }

    bool canAccept() {
        return waitingNum < RCodeParameters::interruptVectorWorkingNum;
    }

    void acceptInterrupt(RCodeBusInterrupt *i) {
        waitingInterrupts[waitingNum++] = i;
    }

    bool hasInterruptSource() {
        return waitingNum > 0;
    }

    RCodeBusInterrupt* takeInterrupt() {
        RCodeBusInterrupt *interrupt = waitingInterrupts[0];
        for (int i = 0; i < waitingNum - 1; i++) {
            waitingInterrupts[i] = waitingInterrupts[i + 1];
        }
        waitingNum--;
        return interrupt;
    }

    RCodeExecutionSpace getSpace() {
        return space;
    }

    bool hasVector(RCodeBusInterrupt *busInt) {
        return vectorMap.hasVector(busInt->getNotificationType(),
                busInt->getNotificationBus(), busInt->getFoundAddress(),
                busInt->getSource()->hasAddress()
                        && busInt->hasFindableAddress());

    }

    executionSpaceAddress_t findVector(RCodeBusInterrupt *busInt) {
        return vectorMap.getVector(busInt->getNotificationType(),
                busInt->getNotificationBus(), busInt->getFoundAddress(),
                busInt->getSource()->hasAddress()
                        && busInt->hasFindableAddress());
    }

    RCodeInterruptVectorOut* getOut() {
        return &out;
    }

    RCodeNotificationManager* getNotificationManager() {
        return notificationManager;
    }
};

#endif /* SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORMANAGER_HPP_ */

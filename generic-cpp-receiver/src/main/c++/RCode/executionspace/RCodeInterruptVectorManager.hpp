/*
 * RCodeInterruptVectorManager.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORMANAGER_HPP_
#define SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORMANAGER_HPP_
#include "../RCodeIncludes.hpp"
#include "../RCodeBusInterrupt.hpp"
#include "RCodeInterruptVectorOut.hpp"
#include "RCodeInterruptVectorMap.hpp"
#include "RCodeExecutionSpace.hpp"

template<class RP>
class RCodeInterruptVectorManager {
    typedef typename RP::executionSpaceAddress_t executionSpaceAddress_t;
    private:
    RCodeNotificationManager<RP> *notificationManager;
    RCodeExecutionSpace<RP> space;
    RCodeInterruptVectorOut<RP> out;
    RCodeInterruptVectorMap<RP> vectorMap;
    RCodeBusInterrupt<RP> waitingInterrupts[RP::interruptVectorWorkingNum];
    int waitingNum = 0;

public:
    RCodeInterruptVectorManager(RCodeNotificationManager<RP> *notificationManager, RCodeExecutionSpace<RP> space) :
            notificationManager(notificationManager), space(space), out(notificationManager) {
    }

    RCodeInterruptVectorMap<RP>* getVectorMap() {
        return &vectorMap;
    }

    bool canAccept() {
        return waitingNum < RP::interruptVectorWorkingNum;
    }

    void acceptInterrupt(RCodeBusInterrupt<RP> i) {
        waitingInterrupts[waitingNum++] = i;
    }

    bool hasInterruptSource() {
        return waitingNum > 0;
    }

    RCodeBusInterrupt<RP> takeInterrupt() {
        RCodeBusInterrupt<RP> interrupt = waitingInterrupts[0];
        for (int i = 0; i < waitingNum - 1; i++) {
            waitingInterrupts[i] = waitingInterrupts[i + 1];
        }
        waitingNum--;
        return interrupt;
    }

    RCodeExecutionSpace<RP> getSpace() {
        return space;
    }

    bool hasVector(RCodeBusInterrupt<RP> *busInt) {
        return vectorMap.hasVector(busInt->getNotificationType(), busInt->getNotificationBus(), busInt->getFoundAddress(),
                busInt->getSource()->hasFindableAddress(busInt->getId()) && busInt->hasFindableAddress());

    }

    executionSpaceAddress_t findVector(RCodeBusInterrupt<RP> *busInt) {
        return vectorMap.getVector(busInt->getNotificationType(), busInt->getNotificationBus(), busInt->getFoundAddress(),
                busInt->getSource()->hasAddress() && busInt->hasFindableAddress());
    }

    RCodeInterruptVectorOut<RP>* getOut() {
        return &out;
    }

    RCodeNotificationManager<RP>* getNotificationManager() {
        return notificationManager;
    }
};

#endif /* SRC_MAIN_CPP_RCODE_EXECUTIONSPACE_RCODEINTERRUPTVECTORMANAGER_HPP_ */

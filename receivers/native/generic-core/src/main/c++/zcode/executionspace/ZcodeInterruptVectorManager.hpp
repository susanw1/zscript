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

template<class ZP>
class ZcodeInterruptVectorManager {
    private:
        typedef typename ZP::executionSpaceAddress_t executionSpaceAddress_t;

        ZcodeNotificationManager<ZP> *notificationManager;
        ZcodeExecutionSpace<ZP> space;
        ZcodeInterruptVectorOut<ZP> out;
        ZcodeInterruptVectorMap<ZP> vectorMap;
        ZcodeBusInterrupt<ZP> waitingInterrupts[ZP::interruptVectorWorkingNum];
        int waitingNum = 0;

    public:
        ZcodeInterruptVectorManager(ZcodeNotificationManager<ZP> *notificationManager, ZcodeExecutionSpace<ZP> space) :
                notificationManager(notificationManager), space(space), out(notificationManager) {
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

        ZcodeBusInterrupt<ZP> takeInterrupt() {
            ZcodeBusInterrupt<ZP> interrupt = waitingInterrupts[0];
            for (int i = 0; i < waitingNum - 1; i++) {
                waitingInterrupts[i] = waitingInterrupts[i + 1];
            }
            waitingNum--;
            return interrupt;
        }

        ZcodeExecutionSpace<ZP> getSpace() {
            return space;
        }

        bool hasVector(ZcodeBusInterrupt<ZP> *busInt) {
            return vectorMap.hasVector(busInt->getNotificationType(), busInt->getNotificationBus(), busInt->getFoundAddress(),
                    busInt->getSource()->hasFindableAddress(busInt->getId()) && busInt->hasFindableAddress());
        }

        executionSpaceAddress_t findVector(ZcodeBusInterrupt<ZP> *busInt) {
            return vectorMap.getVector(busInt->getNotificationType(), busInt->getNotificationBus(), busInt->getFoundAddress(),
                    busInt->getSource()->hasAddress() && busInt->hasFindableAddress());
        }

        ZcodeInterruptVectorOut<ZP>* getOut() {
            return &out;
        }

        ZcodeNotificationManager<ZP>* getNotificationManager() {
            return notificationManager;
        }
};

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMANAGER_HPP_ */

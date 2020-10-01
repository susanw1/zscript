/*
 * RCodeBusInterrupt.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPT_HPP_
#define SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPT_HPP_

#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeBusInterruptSource.hpp"

class RCodeBusInterrupt {
private:
    RCodeBusInterruptSource *source;
    uint8_t id;
public:
    RCodeBusInterrupt(RCodeBusInterruptSource *source, uint8_t id) :
            source(source) {
    }

    RCodeBusInterruptSource* getSource() {
        return source;
    }

    uint8_t getId() {
        return id;
    }

    uint8_t getNotificationType() {
        return source->getNotificationType(id);
    }

    uint8_t getNotificationBus() {
        return source->getNotificationBus(id);
    }

    uint8_t getFoundAddress() {
        return source->getFoundAddress(id);
    }

    bool hasFindableAddress() {
        return source->hasFindableAddress(id);
    }

    void findAddress() {
        source->findAddress(id);
    }

    bool hasStartedAddressFind() {
        return source->hasStartedAddressFind(id);
    }

    bool hasFoundAddress() {
        return source->hasFoundAddress(id);
    }

    bool checkFindAddressLocks() {
        return source->checkFindAddressLocks(id);
    }

    void clear() {
        source->clearNotification(id);
    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPT_HPP_ */

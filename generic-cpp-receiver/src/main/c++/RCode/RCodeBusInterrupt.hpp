/*
 * RCodeBusInterrupt.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPT_HPP_
#define SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPT_HPP_

#include "RCodeIncludes.hpp"
#include "RCodeBusInterruptSource.hpp"

template<class RP>
class RCodeBusInterrupt {
private:
    RCodeBusInterruptSource<RP> *source;
    uint8_t id;

public:

    RCodeBusInterrupt() :
            source(NULL), id(0) {
    }
    RCodeBusInterrupt(RCodeBusInterruptSource<RP> *source, uint8_t id) :
            source(source), id(id) {
    }

    RCodeBusInterruptSource<RP>* getSource() {
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

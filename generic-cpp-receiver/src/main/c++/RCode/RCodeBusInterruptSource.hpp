/*
 * RCodeBusInterruptSource.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPTSOURCE_HPP_
#define SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPTSOURCE_HPP_

#include "RCodeIncludes.hpp"

template<class RP>
class RCodeBusInterruptSource {
public:

    virtual int16_t takeUncheckedNotificationId() = 0;

    virtual uint8_t getNotificationType(uint8_t id) = 0;

    virtual uint8_t getNotificationBus(uint8_t id) = 0;

    virtual void clearNotification(uint8_t id) = 0;

    virtual bool hasFindableAddress(uint8_t id) = 0;

    virtual bool findAddress(uint8_t id) = 0; //returns -1 if cannot, for e.g. locking reasons

    virtual int16_t getFoundAddress(uint8_t id) = 0; //returns -1 if not found yet

    virtual bool hasStartedAddressFind(uint8_t id) = 0;

    virtual ~RCodeBusInterruptSource() {

    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPTSOURCE_HPP_ */

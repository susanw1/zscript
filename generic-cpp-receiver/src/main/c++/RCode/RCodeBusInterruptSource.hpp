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
    virtual bool hasAddress() = 0;

    virtual bool hasUncheckedNotifications() = 0;

    virtual uint8_t takeUncheckedNotificationId() = 0;

    virtual uint8_t getNotificationType(uint8_t id) = 0;

    virtual uint8_t getNotificationBus(uint8_t id) = 0;

    virtual void findAddress(uint8_t id) = 0;

    virtual uint8_t getFoundAddress(uint8_t id) = 0;

    virtual bool hasFindableAddress(uint8_t id) = 0;

    virtual bool hasStartedAddressFind(uint8_t id) = 0;

    virtual bool hasFoundAddress(uint8_t id) = 0;

    virtual bool checkFindAddressLocks(uint8_t id) = 0;

    virtual void clearNotification(uint8_t id) = 0;

    virtual ~RCodeBusInterruptSource() {

    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODEBUSINTERRUPTSOURCE_HPP_ */

/*
 * ZcodeBusInterruptSource.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEBUSINTERRUPTSOURCE_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEBUSINTERRUPTSOURCE_HPP_

#include "ZcodeIncludes.hpp"

struct ZcodeNotificationInfo {
    uint8_t id;
    uint8_t type;
    uint8_t bus;
    bool valid = false; // false if no notifications were available
};

struct ZcodeNotificationAddressInfo {
    uint8_t address;
    bool hasFound;
    bool valid = false;
};

template<class ZP>
class ZcodeBusInterruptSource {
public:
    virtual ZcodeNotificationInfo takeUncheckedNotification() = 0;

    virtual void clearNotification(uint8_t id) = 0;

    virtual ZcodeNotificationAddressInfo getAddressInfo(uint8_t id) = 0;

};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEBUSINTERRUPTSOURCE_HPP_ */

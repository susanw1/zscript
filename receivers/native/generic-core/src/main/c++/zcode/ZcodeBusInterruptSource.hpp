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
    uint8_t id :8;
    uint8_t port :8;
    uint16_t module :12;
    bool valid :1; // false if no notifications were available
};

struct ZcodeNotificationAddressInfo {
    uint16_t address :12;
    bool hasFound :1;
    bool valid :1; // false if no address is available
    bool isAddressed :1;
};

template<class ZP>
class ZcodeBusInterruptSource {
public:
    virtual ZcodeNotificationInfo takeUncheckedNotification() = 0;

    virtual void clearNotification(uint8_t id) = 0;

    virtual ZcodeNotificationAddressInfo getAddressInfo(uint8_t id) {
        (void) id;
        ZcodeNotificationAddressInfo info;
        info.valid = false;
        return info;
    }

};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEBUSINTERRUPTSOURCE_HPP_ */

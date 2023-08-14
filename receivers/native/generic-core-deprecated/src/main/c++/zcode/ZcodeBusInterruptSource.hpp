/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_ZCODEBUSINTERRUPTSOURCE_HPP_
#define SRC_MAIN_CPP_ZCODE_ZCODEBUSINTERRUPTSOURCE_HPP_

#include "ZcodeIncludes.hpp"

struct ZcodeNotificationInfo {
    uint8_t id :8;
    uint8_t port :8;
    uint16_t module :12;
    bool valid :1; // false if no notifications were available
    bool evilBitPackedFlag :1;
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

#endif /* SRC_MAIN_CPP_ZCODE_ZCODEBUSINTERRUPTSOURCE_HPP_ */
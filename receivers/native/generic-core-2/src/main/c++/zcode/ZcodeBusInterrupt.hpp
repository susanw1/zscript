/*
 * ZcodeBusInterrupt.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODEBUSINTERRUPT_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODEBUSINTERRUPT_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeBusInterruptSource.hpp"

template<class ZP>
class ZcodeBusInterrupt {
private:
    ZcodeBusInterruptSource<ZP> *source;
    ZcodeNotificationInfo info;
    ZcodeNotificationAddressInfo address;

public:
    ZcodeBusInterrupt() :
            source(NULL), info(), address() {
    }

    ZcodeBusInterrupt(ZcodeBusInterruptSource<ZP> *source, ZcodeNotificationInfo info, ZcodeNotificationAddressInfo addr) :
            source(source), info(info), address(addr) {
    }

    ZcodeBusInterruptSource<ZP>* getSource() {
        return source;
    }

    uint8_t getNotificationType() {
        return info.type;
    }

    uint8_t getNotificationBus() {
        return info.bus;
    }

    uint16_t getFoundAddress() {
        return address.address;
    }

    bool hasFindableAddress() {
        return address.valid;
    }

    bool hasFoundAddress() {
        return address.hasFound;
    }

    void clear() {
        source->clearNotification(info.id);
        info.valid = false;
        address.valid = false;
        source = NULL;
    }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODEBUSINTERRUPT_HPP_ */

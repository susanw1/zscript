/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_ZCODEBUSINTERRUPT_HPP_
#define SRC_MAIN_CPP_ZCODE_ZCODEBUSINTERRUPT_HPP_

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
        info.evilBitPackedFlag = false;
    }

    ZcodeBusInterruptSource<ZP>* getSource() {
        return source;
    }

    uint16_t getNotificationModule() {
        return info.module;
    }

    uint8_t getNotificationPort() {
        return info.port;
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
    void setFlag(bool b) {
        info.evilBitPackedFlag = b;
    }
    bool getFlag() {
        return info.evilBitPackedFlag;
    }

    bool isValid() {
        return info.valid;
    }
    void invalidate() {
        info.valid = false;
    }
    void refindAddress() {
        address = source->getAddressInfo(info.id);
    }

    void clear() {
        source->clearNotification(info.id);
        info.valid = false;
        address.valid = false;
        info.evilBitPackedFlag = false;
        source = NULL;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_ZCODEBUSINTERRUPT_HPP_ */

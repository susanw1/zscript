/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_ZCODELOCKSET_HPP_
#define SRC_MAIN_CPP_ZCODE_ZCODELOCKSET_HPP_

#include "ZcodeIncludes.hpp"

/**
 * Set of locks being requested.
 */

struct ZcodeLockSetStatus {
    bool locked :1;
    bool nibbleOffset :1;

};
template<class ZP>
class ZcodeLocks;

template<class ZP>
class ZcodeLockSet {
private:
    typedef typename ZP::lockNumber_t lockNumber_t;
    friend class ZcodeLocks<ZP> ;

    uint8_t locks[(ZP::lockNum + 7) / 8];
    lockNumber_t currentSettingPos;
    ZcodeLockSetStatus status;

    void setLocked() {
        status.locked = true;
    }
    void unsetLocked() {
        status.locked = false;
    }
public:
    ZcodeLockSet() {
        for (lockNumber_t i = 0; i < (ZP::lockNum + 7) / 8; ++i) {
            locks[i] = 0;
        }
        status.locked = false;
        status.nibbleOffset = false;
    }

    bool setLocks4(uint8_t locksToSet) {
        if (isAtEnd()) {
            return false;
        }
        if (status.nibbleOffset) {
            locks[currentSettingPos] = (uint8_t) (locks[currentSettingPos] | locksToSet);
            currentSettingPos++;
            status.nibbleOffset = false;
        } else {
            locks[currentSettingPos] = (uint8_t) (locks[currentSettingPos] | (locksToSet << 4));
            status.nibbleOffset = true;
        }
        return true;
    }

    bool setLocks8(uint8_t locks) {
        if (isAtEnd()) {
            return false;
        }
        locks[currentSettingPos++] = locks;
        return true;
    }

    bool isAtEnd() {
        return currentSettingPos >= (ZP::lockNum + 7) / 8;
    }

    const uint8_t* getLocks() const {
        return locks;
    }

    bool isLocked() const {
        return status.locked;
    }
    bool canLock() {
        return Zcode<ZP>::zcode.getLocks()->canLock(this);
    }
    bool lock() {
        if (status.locked) {
            return true;
        }
        return Zcode<ZP>::zcode.getLocks()->lock(this);
    }
    void unlock() {
        if (status.locked) {
            Zcode<ZP>::zcode.getLocks()->unlock(this);
        }
    }
    void reset() {
        if (isLocked()) {
            unlock();
        }
        currentSettingPos = 0;
    }

    void clearAll() {
        for (lockNumber_t i = 0; i < (ZP::lockNum + 7) / 8; ++i) {
            locks[i] = 0;
        }
    }
    void setAll() {
        for (lockNumber_t i = 0; i < (ZP::lockNum + 7) / 8; ++i) {
            locks[i] = 0xFF;
        }
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_ZCODELOCKSET_HPP_ */

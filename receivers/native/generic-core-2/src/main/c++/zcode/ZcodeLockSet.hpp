/*
 * ZcodeLockSet.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODELOCKSET_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODELOCKSET_HPP_

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

    ZcodeLocks<ZP> *lockAgainst;
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
    ZcodeLockSet(ZcodeLocks<ZP> *lockAgainst) :
            lockAgainst(lockAgainst) {
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
        return lockAgainst->canLock(this);
    }
    bool lock() {
        if (status.locked) {
            return true;
        }
        return lockAgainst->lock(this);
    }
    void unlock() {
        if (status.locked) {
            lockAgainst->unlock(this);
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

#endif /* SRC_TEST_CPP_ZCODE_ZCODELOCKSET_HPP_ */

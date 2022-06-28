/*
 * ZcodeLocks.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODELOCKS_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODELOCKS_HPP_

#include "ZcodeIncludes.hpp"
#include "ZcodeLockSet.hpp"

/**
 * Authoritative central store of locks.
 */
template<class ZP>
class ZcodeLocks {
private:
    typedef typename ZP::lockNumber_t lockNumber_t;

    /** Bitset of boolean locks */
    uint8_t locks[(ZP::lockNum + 7) / 8];

public:
    ZcodeLocks() {
        for (lockNumber_t i = 0; i < (ZP::lockNum + 7) / 8; ++i) {
            locks[i] = 0;
        }
    }

    bool canLock(ZcodeLockSet<ZP> *l) {
        const uint8_t *lockSet = l->getLocks();
        for (lockNumber_t i = 0; i < (ZP::lockNum + 7) / 8; i++) {
            if ((locks[i] & lockSet[i]) != 0) {
                return false;
            }
        }
        return true;
    }

    bool lock(ZcodeLockSet<ZP> *l) {
        const uint8_t *lockSet = l->getLocks();
        for (lockNumber_t i = 0; i < (ZP::lockNum + 7) / 8; i++) {
            if ((locks[i] & lockSet[i]) == 0) {
                locks[i] = (uint8_t) (locks[i] | lockSet[i]);
            } else {
                for (lockNumber_t j = 0; j < i; j++) {
                    locks[j] = (uint8_t) (locks[j] & ~lockSet[j]);
                }
                return false;
            }
        }
        l->setLocked();
        return true;
    }

    void unlock(ZcodeLockSet<ZP> *l) {
        const uint8_t *lockSet = l->getLocks();
        for (lockNumber_t i = 0; i < (ZP::lockNum + 7) / 8; i++) {
            locks[i] = (uint8_t) (locks[i] & ~lockSet[i]);
        }
        l->unsetLocked();
    }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODELOCKS_HPP_ */

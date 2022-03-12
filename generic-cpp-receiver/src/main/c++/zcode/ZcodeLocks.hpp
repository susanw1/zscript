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
template<class RP>
class ZcodeLocks {
private:
    /** Bitset of boolean locks */
    uint8_t locks[(RP::highestBasicLockNum + 7) / 8];

    /** Byte that represents value of the RW locks, 255 means write-locked */
    uint8_t rwLocks[RP::highestRwLockNum - RP::lowestRwLockNum];

public:
    ZcodeLocks() {
        for (int i = 0; i < (RP::highestBasicLockNum + 7) / 8; ++i) {
            locks[i] = 0;
        }
        for (int i = 0; i < RP::highestRwLockNum - RP::lowestRwLockNum; ++i) {
            rwLocks[i] = 0;
        }
    }

    bool canLock(ZcodeLockSet<RP> *l) {
        for (int i = 0; i < l->getLockNum(); i++) {
            if (l->getLocks()[i] < RP::highestBasicLockNum) {
                if ((locks[l->getLocks()[i] / 8] & (1 << (l->getLocks()[i] % 8))) != 0) {
                    return false;
                }
            } else if (l->getLocks()[i] < RP::highestRwLockNum && l->getLocks()[i] >= RP::lowestRwLockNum) {
                if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    if (rwLocks[l->getLocks()[i] - RP::lowestRwLockNum] != 0) {
                        return false;
                    }
                } else {
                    if (rwLocks[l->getLocks()[i] - RP::lowestRwLockNum] == 255) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void setBits(uint8_t *p, uint8_t val) {
        *p |= val;
    }

    void clearBits(uint8_t *p, uint8_t val) {
        *p &= (uint8_t) ~val;
    }

    void lock(ZcodeLockSet<RP> *l) {
        // FIXME: uint8_t casts everywhere! Argh!
        for (int i = 0; i < l->getLockNum(); i++) {
            uint8_t lockToSet = l->getLocks()[i];

            if (lockToSet < RP::highestBasicLockNum) {
                setBits(&locks[lockToSet / 8], (uint8_t)(1 << (lockToSet % 8)));
            } else if (lockToSet < RP::highestRwLockNum && lockToSet >= RP::lowestRwLockNum) {
                if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    rwLocks[lockToSet - RP::lowestRwLockNum] = 255;
                } else if (rwLocks[lockToSet - RP::lowestRwLockNum] != 255) {
                    rwLocks[lockToSet - RP::lowestRwLockNum]++;
                }
            }
        }
    }

    void unlock(ZcodeLockSet<RP> *l) {
        for (int i = 0; i < l->getLockNum(); i++) {
            uint8_t lockToClear = l->getLocks()[i];
            if (lockToClear < RP::highestBasicLockNum) {
                clearBits(&locks[lockToClear / 8], (uint8_t)(1 << (lockToClear % 8)));
            } else if (lockToClear < RP::highestRwLockNum && lockToClear >= RP::lowestRwLockNum) {
                if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    rwLocks[lockToClear - RP::lowestRwLockNum] = 0;
                } else if (rwLocks[lockToClear - RP::lowestRwLockNum] != 0) {
                    rwLocks[lockToClear - RP::lowestRwLockNum]--;
                }
            }
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODELOCKS_HPP_ */

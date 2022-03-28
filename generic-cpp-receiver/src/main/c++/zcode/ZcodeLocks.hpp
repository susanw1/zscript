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
        /** Bitset of boolean locks */
        uint8_t locks[(ZP::highestBasicLockNum + 7) / 8];

        /** Byte that represents value of the RW locks, 255 means write-locked */
        uint8_t rwLocks[ZP::highestRwLockNum - ZP::lowestRwLockNum];

    public:
        ZcodeLocks() {
            for (int i = 0; i < (ZP::highestBasicLockNum + 7) / 8; ++i) {
                locks[i] = 0;
            }
            for (int i = 0; i < ZP::highestRwLockNum - ZP::lowestRwLockNum; ++i) {
                rwLocks[i] = 0;
            }
        }

        bool canLock(ZcodeLockSet<ZP> *l) {
            for (int i = 0; i < l->getLockNum(); i++) {
                if (l->getLocks()[i] < ZP::highestBasicLockNum) {
                    if ((locks[l->getLocks()[i] / 8] & (1 << (l->getLocks()[i] % 8))) != 0) {
                        return false;
                    }
                } else if (l->getLocks()[i] < ZP::highestRwLockNum && l->getLocks()[i] >= ZP::lowestRwLockNum) {
                    if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                        if (rwLocks[l->getLocks()[i] - ZP::lowestRwLockNum] != 0) {
                            return false;
                        }
                    } else {
                        if (rwLocks[l->getLocks()[i] - ZP::lowestRwLockNum] == 255) {
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

        void lock(ZcodeLockSet<ZP> *l) {
            // FIXME: uint8_t casts everywhere! Argh!
            for (int i = 0; i < l->getLockNum(); i++) {
                uint8_t lockToSet = l->getLocks()[i];

                if (lockToSet < ZP::highestBasicLockNum) {
                    setBits(&locks[lockToSet / 8], (uint8_t) (1 << (lockToSet % 8)));
                } else if (lockToSet < ZP::highestRwLockNum && lockToSet >= ZP::lowestRwLockNum) {
                    if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                        rwLocks[lockToSet - ZP::lowestRwLockNum] = 255;
                    } else if (rwLocks[lockToSet - ZP::lowestRwLockNum] != 255) {
                        rwLocks[lockToSet - ZP::lowestRwLockNum]++;
                    }
                }
            }
        }

        void unlock(ZcodeLockSet<ZP> *l) {
            for (int i = 0; i < l->getLockNum(); i++) {
                uint8_t lockToClear = l->getLocks()[i];
                if (lockToClear < ZP::highestBasicLockNum) {
                    clearBits(&locks[lockToClear / 8], (uint8_t) (1 << (lockToClear % 8)));
                } else if (lockToClear < ZP::highestRwLockNum && lockToClear >= ZP::lowestRwLockNum) {
                    if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                        rwLocks[lockToClear - ZP::lowestRwLockNum] = 0;
                    } else if (rwLocks[lockToClear - ZP::lowestRwLockNum] != 0) {
                        rwLocks[lockToClear - ZP::lowestRwLockNum]--;
                    }
                }
            }
        }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODELOCKS_HPP_ */

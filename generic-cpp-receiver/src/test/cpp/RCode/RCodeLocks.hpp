/*
 * RCodeLocks.hpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODELOCKS_HPP_
#define SRC_TEST_CPP_RCODE_RCODELOCKS_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeLockSet.hpp"

class RCodeLocks {

private:
    uint8_t locks[(RCodeParameters::highestBasicLockNum + 7) / 8];
    uint8_t rwLocks[RCodeParameters::highestRwLockNum
            - RCodeParameters::lowestRwLockNum];

public:
    RCodeLocks() {
        for (int i = 0; i < (RCodeParameters::highestBasicLockNum + 7) / 8;
                ++i) {
            locks[i] = 0;
        }
        for (int i = 0;
                i
                        < RCodeParameters::highestRwLockNum
                                - RCodeParameters::lowestRwLockNum; ++i) {
            rwLocks[i] = 0;
        }
    }
    bool canLock(RCodeLockSet *l) {
        for (int i = 0; i < l->getLockNum(); i++) {
            if (l->getLocks()[i] < RCodeParameters::highestBasicLockNum) {
                if ((locks[l->getLocks()[i] / 8] & (1 << (l->getLocks()[i] % 8)))
                        != 0) {
                    return false;
                }
            } else if (l->getLocks()[i] < RCodeParameters::highestRwLockNum
                    && l->getLocks()[i] >= RCodeParameters::lowestRwLockNum) {
                if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    if (rwLocks[l->getLocks()[i]
                            - RCodeParameters::lowestRwLockNum] != 0) {
                        return false;
                    }
                } else {
                    if (rwLocks[l->getLocks()[i]
                            - RCodeParameters::lowestRwLockNum] == 255) {
                        return false;
                    }
                }
            }
        }
        return true;
    }

    void lock(RCodeLockSet *l) {
        for (int i = 0; i < l->getLockNum(); i++) {
            if (l->getLocks()[i] < RCodeParameters::highestBasicLockNum) {
                locks[l->getLocks()[i] / 8] |= 1 << (l->getLocks()[i] % 8);
            } else if (l->getLocks()[i] < RCodeParameters::highestRwLockNum
                    && l->getLocks()[i] >= RCodeParameters::lowestRwLockNum) {
                if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    rwLocks[l->getLocks()[i] - RCodeParameters::lowestRwLockNum] =
                            255;
                } else if (rwLocks[l->getLocks()[i]
                        - RCodeParameters::lowestRwLockNum] != 255) {
                    rwLocks[l->getLocks()[i] - RCodeParameters::lowestRwLockNum]++;
                }
            }
        }
    }

    void unlock(RCodeLockSet *l) {
        for (int i = 0; i < l->getLockNum(); i++) {
            if (l->getLocks()[i] < RCodeParameters::highestBasicLockNum) {
                locks[l->getLocks()[i] / 8] &= ~(1 << (l->getLocks()[i] % 8));
            } else if (l->getLocks()[i] < RCodeParameters::highestRwLockNum
                    && l->getLocks()[i] >= RCodeParameters::lowestRwLockNum) {
                if ((l->getW_nr()[i / 8] & 1 << i % 8) != 0) {
                    rwLocks[l->getLocks()[i] - RCodeParameters::lowestRwLockNum] =
                            0;
                } else if (rwLocks[l->getLocks()[i]
                        - RCodeParameters::lowestRwLockNum] != 0) {
                    rwLocks[l->getLocks()[i] - RCodeParameters::lowestRwLockNum]--;
                }
            }
        }
    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODELOCKS_HPP_ */

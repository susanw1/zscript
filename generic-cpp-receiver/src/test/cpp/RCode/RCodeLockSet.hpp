/*
 * RCodeLockSet.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODELOCKSET_HPP_
#define SRC_TEST_CPP_RCODE_RCODELOCKSET_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"

class RCodeLockSet {
private:
    uint8_t locks[RCodeParameters::lockNum];
    uint8_t w_nr[RCodeParameters::lockNum];
    bool active = false;
    uint16_t lockNum = 0;

public:
    void addLock(uint8_t lock, bool isWrite) {
        for (int i = 0; i < lockNum; i++) {
            if (locks[i] == lock) {
                if (isWrite) {
                    w_nr[i / 8] |= 1 << i % 8;
                }
                break;
            }
        }
        locks[lockNum] = lock;
        if (isWrite) {
            w_nr[lockNum / 8] |= 1 << lockNum % 8;
        }
        lockNum++;
    }

    uint8_t const* getLocks() const {
        return locks;
    }

    uint16_t getLockNum() const {
        return lockNum;
    }

    uint8_t const* getW_nr() const {
        return w_nr;
    }
    bool isActive() const {
        return active;
    }
    void activate() {
        active = true;
    }
    void reset() {
        active = false;
        lockNum = 0;
    }
};

#endif /* SRC_TEST_CPP_RCODE_RCODELOCKSET_HPP_ */

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
template<class RP>
class ZcodeLockSet {
    typedef typename RP::lockNumber_t lockNumber_t;
private:
    // FIXME: locks are lockIds
    uint8_t locks[RP::lockNum];
    uint8_t w_nr[(RP::lockNum + 7) / 8];
    bool active = false;
    lockNumber_t lockNum = 0;

public:
    void addLock(uint8_t lock, bool isWrite) {
        // FIXME: check overrun
        for (int i = 0; i < lockNum; i++) {
            if (locks[i] == lock) {
                if (isWrite) {
                    w_nr[i / 8] |= (uint8_t)(1 << i % 8);
                }
                break;
            }
        }
        locks[lockNum] = lock;
        if (isWrite) {
            w_nr[lockNum / 8] |= (uint8_t)(1 << lockNum % 8);
        }
        lockNum++;
    }

    const uint8_t* getLocks() const {
        return locks;
    }

    lockNumber_t getLockNum() const {
        return lockNum;
    }

    const uint8_t* getW_nr() const {
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

#endif /* SRC_TEST_CPP_ZCODE_ZCODELOCKSET_HPP_ */

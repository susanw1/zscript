/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_LOCKSET_HPP_
#define SRC_MAIN_C___ZSCRIPT_LOCKSET_HPP_
#include "../ZscriptIncludes.hpp"

namespace Zscript {
namespace GenericCore {
template<class ZP>
class LockSet {
private:
    uint8_t locks[ZP::lockByteCount];

    LockSet(CombinedTokenBlockIterator<ZP> iterator) {
        for (uint8_t i = 0; i < ZP::lockByteCount; ++i) {
            this->locks[i] = iterator.next();
        }
    }
    LockSet(uint8_t *locks) {
        for (uint8_t i = 0; i < ZP::lockByteCount; ++i) {
            this->locks[i] = locks[i];
        }
    }

    LockSet(uint8_t lock) {
        for (uint8_t i = 0; i < ZP::lockByteCount; ++i) {
            this->locks[i] = lock;
        }
    }

public:

    static LockSet empty() {
        return LockSet(0x00);
    }

    static LockSet allLocked() {
        return LockSet(0xFF);
    }
    static LockSet from(CombinedTokenBlockIterator<ZP> iterator) {
        return LockSet(iterator);
    }

    uint8_t* getLocks() {
        return locks;
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_LOCKSET_HPP_ */

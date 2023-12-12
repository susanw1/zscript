/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_C___ZSCRIPT_LOCKSYSTEM_HPP_
#define SRC_MAIN_C___ZSCRIPT_LOCKSYSTEM_HPP_

#include "../ZscriptIncludes.hpp"
#include "LockSet.hpp"

namespace Zscript {
namespace GenericCore {

template<class ZP>
class LockSystem {
    uint8_t locks[ZP::lockByteCount];

public:
    LockSystem() {
        for (int i = 0; i < ZP::lockByteCount; i++) {
            locks[i] = 0;
        }
    }

    bool lock(const LockSet<ZP> *l) {
        if (!canLock(l)) {
            return false;
        }
        const uint8_t *toApply = l->getLocks();
        for (int i = 0; i < ZP::lockByteCount; i++) {
            locks[i] |= toApply[i];
        }
        return true;
    }

    bool canLock(const LockSet<ZP> *l) {
        const uint8_t *toApply = l->getLocks();
        for (int i = 0; i < ZP::lockByteCount; i++) {
            if ((locks[i] & toApply[i]) != 0) {
                return false;
            }
        }
        return true;
    }

    void unlock(const LockSet<ZP> *l) {
        const uint8_t *toApply = l->getLocks();
        for (int i = 0; i < ZP::lockByteCount; i++) {
            locks[i] &= ~toApply[i];
        }
    }
};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_LOCKSYSTEM_HPP_ */

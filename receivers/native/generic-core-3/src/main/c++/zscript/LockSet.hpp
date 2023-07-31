/*
 * LockSet.hpp
 *
 *  Created on: 31 Jul 2023
 *      Author: alicia
 */

#ifndef SRC_MAIN_C___ZSCRIPT_LOCKSET_HPP_
#define SRC_MAIN_C___ZSCRIPT_LOCKSET_HPP_
#include "ZscriptIncludes.hpp"

namespace Zscript {
namespace GenericCore {
template<class ZP>
class LockSet {
private:
    uint8_t locks[ZP::lockSize];

    LockSet(RawTokenBlockIterator<ZP> iterator, TokenRingBuffer<ZP> *buffer) {
        for (uint8_t i = 0; i < ZP::lockByteCount; ++i) {
            this->locks[i] = iterator.next(buffer);
        }
    }
    LockSet(uint8_t *locks) {
        for (uint8_t i = 0; i < ZP::lockByteCount; ++i) {
            this->locks[i] = locks[i];
        }
    }

    LockSet(uint8_t lock) {
        for (uint8_t i = 0; i < ZP::lockByteCount; ++i) {
            this->locks[i] = locks;
        }
    }

public:
    //    static ZcodeLockSet from() {
//        byte[] locks = new byte[ZcodeLocks.LOCK_BYTENUM];
//
//        int i = 0;
//        for (Iterator < Byte > iterator = data; i < ZcodeLocks.LOCK_BYTENUM && iterator.hasNext();) {
//            locks[i++] = iterator.next();
//        }
//        return new ZcodeLockSet(locks);
//    }

    static LockSet empty() {
        return LockSet(0x00);
    }

    static LockSet allLocked() {
        return LockSet(0xFF);
    }
    static LockSet from(RawTokenBlockIterator<ZP> iterator, TokenRingBuffer<ZP> *buffer) {
        return LockSet(iterator, buffer);
    }

    uint8_t* getLocks() {
        return locks;
    }

};
}
}

#endif /* SRC_MAIN_C___ZSCRIPT_LOCKSET_HPP_ */

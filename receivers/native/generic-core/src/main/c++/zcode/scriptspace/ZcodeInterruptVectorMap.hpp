/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODEINTERRUPTVECTORMAP_HPP_
#define SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODEINTERRUPTVECTORMAP_HPP_

#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeInterruptVectorMap {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;

    uint16_t vectorNum = 0;
    uint16_t mostRecent = 0;
    scriptSpaceAddress_t vectors[ZP::maxInterruptScripts];
    uint16_t addresses[ZP::maxInterruptScripts];
    uint8_t specificities[ZP::maxInterruptScripts];
    uint8_t buses[ZP::maxInterruptScripts];
    uint16_t types[ZP::maxInterruptScripts];

    bool matches(uint16_t i, uint16_t type, uint8_t bus, uint16_t addr, uint8_t specificity) {
        return specificities[i] == specificity
                && (specificity == 0 || (types[i] == type && (specificity == 1 || (buses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))));
    }

    bool setVectorInternal(uint16_t type, uint8_t bus, uint16_t addr, uint8_t specificity, scriptSpaceAddress_t vector) {
        for (uint16_t i = 0; i < vectorNum; i++) {
            if (matches(i, type, bus, addr, specificity)) {
                vectors[i] = vector;
                return true;
            }
        }
        if (vectorNum >= ZP::maxInterruptScripts) {
            return false;
        }
        specificities[vectorNum] = specificity;
        addresses[vectorNum] = addr;
        buses[vectorNum] = bus;
        types[vectorNum] = type;
        vectors[vectorNum] = vector;
        vectorNum++;
        return true;
    }

    bool hasVectorInternal(uint16_t type, uint8_t bus, uint16_t addr, uint8_t specificity) {
        if (matches(mostRecent, type, bus, addr, specificity)) {
            return true;
        }
        for (uint16_t i = 0; i < vectorNum; i++) {
            if (matches(i, type, bus, addr, specificity)) {
                mostRecent = i;
                return true;
            }
        }
        return specificity == 0 ? false : hasVectorInternal(type, bus, addr, (uint8_t) (specificity - 1));
    }

    scriptSpaceAddress_t getVectorInternal(uint16_t type, uint8_t bus, uint16_t addr, uint8_t specificity) {
        if (matches(mostRecent, type, bus, addr, specificity)) {
            return vectors[mostRecent];
        }
        for (uint16_t i = 0; i < vectorNum; i++) {
            if (matches(i, type, bus, addr, specificity)) {
                mostRecent = i;
                return vectors[i];
            }
        }
        return specificity == 0 ? 0 : getVectorInternal(type, bus, addr, (uint8_t) (specificity - 1));
    }

public:
    bool setDefaultVector(scriptSpaceAddress_t vector) {
        return setVectorInternal(0, 0, 0, 0, vector);
    }

    bool setVector(uint16_t type, scriptSpaceAddress_t vector) {
        return setVectorInternal(type, 0, 0, 0x01, vector);
    }
    bool setVector(uint16_t type, uint8_t bus, scriptSpaceAddress_t vector) {
        return setVectorInternal(type, bus, 0, 0x02, vector);
    }
    bool setVector(uint16_t type, uint8_t bus, uint16_t addr, scriptSpaceAddress_t vector) {
        return setVectorInternal(type, bus, addr, 0x03, vector);
    }

    bool hasVector(uint16_t type, uint8_t bus, uint16_t addr, bool hasAddress) {
        return hasVectorInternal(type, bus, addr, hasAddress ? 0x03 : 0x02);
    }

    scriptSpaceAddress_t getVector(uint16_t type, uint8_t bus, uint16_t addr, bool hasAddress) {
        return getVectorInternal(type, bus, addr, hasAddress ? 0x03 : 0x02);
    }

};

#endif /* SRC_MAIN_CPP_ZCODE_SCRIPTSPACE_ZCODEINTERRUPTVECTORMAP_HPP_ */

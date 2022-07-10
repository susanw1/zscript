/*
 * ZcodeInterruptVectorMap.hpp
 *
 *  Created on: 9 Dec 2020
 *      Author: robert
 */

#ifndef SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMAP_HPP_
#define SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMAP_HPP_

#include "../ZcodeIncludes.hpp"

template<class ZP>
class ZcodeInterruptVectorMap {
private:
    typedef typename ZP::scriptSpaceAddress_t scriptSpaceAddress_t;

    uint16_t vectorNum = 0;
    uint16_t mostRecent = 0;
    scriptSpaceAddress_t vectors[ZP::interruptVectorNum];
    uint16_t addresses[ZP::interruptVectorNum];
    uint8_t specificities[ZP::interruptVectorNum];
    uint8_t buses[ZP::interruptVectorNum];
    uint8_t types[ZP::interruptVectorNum];

    bool matches(uint16_t i, uint8_t type, uint8_t bus, uint16_t addr, uint8_t specificity) {
        return specificities[i] == specificity
                && (specificity == 0 || (types[i] == type && (specificity == 1 || (buses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))));
    }

    bool setVectorInternal(uint8_t type, uint8_t bus, uint16_t addr, uint8_t specificity, scriptSpaceAddress_t vector) {
        for (uint16_t i = 0; i < vectorNum; i++) {
            if (matches(i, type, bus, addr, specificity)) {
                vectors[i] = vector;
                return true;
            }
        }
        if (vectorNum >= ZP::interruptVectorNum) {
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

    bool hasVectorInternal(uint8_t type, uint8_t bus, uint16_t addr, uint8_t specificity) {
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

    scriptSpaceAddress_t getVectorInternal(uint8_t type, uint8_t bus, uint16_t addr, uint8_t specificity) {
        if (matches(mostRecent, type, bus, addr, specificity)) {
            return vectors[mostRecent];
        }
        for (uint16_t i = 0; i < vectorNum; i++) {
            if (matches(i, type, bus, addr, specificity)) {
                mostRecent = i;
                return vectors[i];
            }
        }
        return specificity == 0 ? 0 : getVectorInternal(type, bus, addr, specificity - 1);
    }

public:
    bool setDefaultVector(scriptSpaceAddress_t vector) {
        return setVectorInternal(0, 0, 0, 0, vector);
    }

    bool setVector(uint8_t type, scriptSpaceAddress_t vector) {
        return setVectorInternal(type, 0, 0, 0x01, vector);
    }
    bool setVector(uint8_t type, uint8_t bus, scriptSpaceAddress_t vector) {
        return setVectorInternal(type, bus, 0, 0x02, vector);
    }
    bool setVector(uint8_t type, uint8_t bus, uint16_t addr, scriptSpaceAddress_t vector) {
        return setVectorInternal(type, bus, addr, 0x03, vector);
    }

    bool hasVector(uint8_t type, uint8_t bus, uint16_t addr, bool hasAddress) {
        return hasVectorInternal(type, bus, addr, hasAddress ? 0x03 : 0x02);
    }

    scriptSpaceAddress_t getVector(uint8_t type, uint8_t bus, uint16_t addr, bool hasAddress) {
        return getVectorInternal(type, bus, addr, hasAddress ? 0x03 : 0x02);
    }

};

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMAP_HPP_ */

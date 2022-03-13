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
    typedef typename ZP::executionSpaceAddress_t executionSpaceAddress_t;
private:
    uint16_t vectorNum = 0;
    uint16_t mostRecent = 0;
    executionSpaceAddress_t vectors[ZP::interruptVectorNum];
    uint8_t addresses[ZP::interruptVectorNum];
    uint8_t specificities[ZP::interruptVectorNum];
    uint8_t busses[ZP::interruptVectorNum];
    uint8_t types[ZP::interruptVectorNum];

    bool setVectorInternal(uint8_t type, uint8_t bus, uint8_t addr, uint8_t specificity, executionSpaceAddress_t vector) {
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || (types[i] == type && (specificity == 1 || (busses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))))) {
                vectors[i] = vector;
                return true;
            }
        }
        if (vectorNum >= ZP::interruptVectorNum) {
            return false;
        }
        specificities[vectorNum] = specificity;
        addresses[vectorNum] = addr;
        busses[vectorNum] = bus;
        types[vectorNum] = type;
        vectors[vectorNum] = vector;
        vectorNum++;
        return true;
    }

    bool hasVectorInternal(uint8_t type, uint8_t bus, uint8_t addr, uint8_t specificity) {
        if (specificities[mostRecent] == specificity
                && (specificity == 0
                        || (types[mostRecent] == type
                                && (specificity == 1 || (busses[mostRecent] == bus && (specificity == 0x02 || addresses[mostRecent] == addr)))))) {
            return true;
        }
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || (types[i] == type && (specificity == 1 || (busses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))))) {
                mostRecent = i;
                return true;
            }
        }
        return specificity == 0 ? false : hasVectorInternal(type, bus, addr, specificity - 1);
    }

    executionSpaceAddress_t getVectorInternal(uint8_t type, uint8_t bus, uint8_t addr, uint8_t specificity) {
        if (specificities[mostRecent] == specificity
                && (specificity == 0
                        || (types[mostRecent] == type
                                && (specificity == 1 || (busses[mostRecent] == bus && (specificity == 0x02 || addresses[mostRecent] == addr)))))) {
            return vectors[mostRecent];
        }
        for (int i = 0; i < vectorNum; i++) {
            if (specificities[i] == specificity
                    && (specificity == 0 || (types[i] == type && (specificity == 1 || (busses[i] == bus && (specificity == 0x02 || addresses[i] == addr)))))) {
                mostRecent = i;
                return vectors[i];
            }
        }
        return specificity == 0 ? 0 : getVectorInternal(type, bus, addr, specificity - 1);
    }

public:

    bool setDefaultVector(executionSpaceAddress_t vector) {
        return setVectorInternal(0, 0, 0, 0, vector);
    }

    bool setVector(uint8_t type, executionSpaceAddress_t vector) {
        return setVectorInternal(type, 0, 0, 0x01, vector);
    }

    bool setVector(uint8_t type, uint8_t bus, uint8_t addr, bool hasAddress, executionSpaceAddress_t vector) {
        return setVectorInternal(type, bus, addr, hasAddress ? 0x03 : 0x02, vector);
    }

    bool hasVector(uint8_t type, uint8_t bus, uint8_t addr, bool hasAddress) {
        return hasVectorInternal(type, bus, addr, hasAddress ? 0x03 : 0x02);
    }

    executionSpaceAddress_t getVector(uint8_t type, uint8_t bus, uint8_t addr, bool hasAddress) {
        return getVectorInternal(type, bus, addr, hasAddress ? 0x03 : 0x02);
    }

};

#endif /* SRC_MAIN_CPP_ZCODE_EXECUTIONSPACE_ZCODEINTERRUPTVECTORMAP_HPP_ */

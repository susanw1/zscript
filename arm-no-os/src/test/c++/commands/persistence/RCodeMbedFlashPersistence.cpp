/*
 * RCodeMbedFlashPersistence.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "RCodeMbedFlashPersistence.hpp"

static const uint16_t guidLocation = 0;
static const uint16_t macAddressLocation = 20;
static const uint16_t mainPersistentMemoryLocation = 32;

mbed::FlashIAP flash;

RCodeMbedFlashPersistence::RCodeMbedFlashPersistence() :
        persistenceStartAddress(
                (((uint32_t) FLASHIAP_APP_ROM_END_ADDR)
                        + flash.get_sector_size(FLASHIAP_APP_ROM_END_ADDR)
                        - ((uint32_t) FLASHIAP_APP_ROM_END_ADDR)
                                % flash.get_sector_size(
                                        FLASHIAP_APP_ROM_END_ADDR))), sectorSize(
                flash.get_sector_size(persistenceStartAddress)) {
    flash.init();
}
uint32_t RCodeMbedFlashPersistence::getStuff() {
    return persistenceStartAddress;
}
uint8_t* RCodeMbedFlashPersistence::getGuid() {
    return (uint8_t*) (persistenceStartAddress) + guidLocation;
}
uint8_t* RCodeMbedFlashPersistence::getMac() {
    return (uint8_t*) (persistenceStartAddress) + macAddressLocation;
}
uint8_t* RCodeMbedFlashPersistence::getPersistentMemory() {
    return (uint8_t*) (persistenceStartAddress) + mainPersistentMemoryLocation;
}
int RCodeMbedFlashPersistence::writeGuid(const uint8_t *guid) {
    return writePersistentInternal(guidLocation, guid, 16);
}
int RCodeMbedFlashPersistence::writeMac(const uint8_t *mac) {
    return writePersistentInternal(macAddressLocation, mac, 6);
}
int RCodeMbedFlashPersistence::writePersistent(uint8_t location,
        const uint8_t *memory, uint8_t length) {
    return writePersistentInternal(location + mainPersistentMemoryLocation,
            memory, length);
}
int RCodeMbedFlashPersistence::writePersistentInternal(uint16_t location,
        const uint8_t *toWrite, uint16_t length) {
    uint8_t page[sectorSize];
    for (int i = 0; i < sectorSize; ++i) {
        page[i] = *((uint8_t*) (persistenceStartAddress) + location + i);
    }

    if (flash.erase(persistenceStartAddress, sectorSize) < 0) {
        return -1;
    }
    for (int i = 0; i < length; ++i) {
        page[location + i] = toWrite[i];
    }
    return flash.program(page, persistenceStartAddress, sectorSize);
}
//        uint16_t startSector = location - location % sectorSize;
//        uint16_t endSector = location + length
//                - (location + length) % sectorSize;
//        if (startSector == endSector) {
//            return 0;
//            uint8_t sector[sectorSize];
//            for (uint16_t i = 0; i < sectorSize; i++) {
//                if (i >= location && i < location + length) {
//                    sector[i + location] = toWrite[i];
//                } else {
//                    sector[i] = persistenceStartAddress[i];
//                }
//            }
//            return rewriteSector(startSector, sector);
//        } else {
//            return -1;
//            uint8_t sector[sectorSize];
//            for (uint16_t i = 0; i < sectorSize; i++) {
//                if (i >= location) {
//                    sector[i + location] = toWrite[i];
//                } else {
//                    sector[i] = persistenceStartAddress[i];
//                }
//            }
//            if (rewriteSector(startSector, sector) < 0) {
//                return -1;
//            }
//            for (uint16_t j = startSector + sectorSize; j < endSector; j +=
//                    sectorSize) {
//                for (uint16_t i = 0; i < sectorSize; i++) {
//                    sector[i] = toWrite[i + j + location % sectorSize];
//                }
//                if (rewriteSector(j, sector) < 0) {
//                    return -1;
//                }
//            }
//            for (uint16_t i = 0; i < sectorSize; i++) {
//                if (i < location + length - endSector) {
//                    sector[i + location] = toWrite[i + endSector
//                            + location % sectorSize];
//                } else {
//                    sector[i] = persistenceStartAddress[endSector + i];
//                }
//            }
//            return rewriteSector(endSector, sector);
//        }

//int RCodeMbedFlashPersistence::rewriteSector(uint16_t start,
//        const uint8_t *toWrite) {
//    if (flash.erase(((uint32_t) persistenceStartAddress) + start, sectorSize)
//            < 0) {
//        return -1;
//    }
//    return flash.program(toWrite, ((uint32_t) persistenceStartAddress) + start,
//            sectorSize);
//}

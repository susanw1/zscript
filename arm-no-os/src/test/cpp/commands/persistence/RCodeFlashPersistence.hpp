/*
 * RCodeFlashPersistence.hpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_RCODEMBEDFLASHPERSISTENCE_HPP_
#define SRC_TEST_CPP_COMMANDS_RCODEMBEDFLASHPERSISTENCE_HPP_

#include <RCodeIncludes.hpp>
#include <RCodeParameters.hpp>
#include <FlashIAP.h>

class RCodeFlashPersistence {
private:
    uint32_t persistenceStartAddress;
    uint16_t sectorSize;

public:
    RCodeFlashPersistence();
    uint32_t getStuff();
    uint8_t* getGuid();

    uint8_t* getMac();

    uint8_t* getPersistentMemory();

    int writeGuid(const uint8_t *guid);

    int writeMac(const uint8_t *mac);

    int writePersistent(uint8_t location, const uint8_t *memory,
            uint8_t length);
    private:
    int writePersistentInternal(uint16_t location, const uint8_t *toWrite,
            uint16_t length);
//    int rewriteSector(uint16_t start, const uint8_t *toWrite);

};

#endif /* SRC_TEST_CPP_COMMANDS_RCODEMBEDFLASHPERSISTENCE_HPP_ */

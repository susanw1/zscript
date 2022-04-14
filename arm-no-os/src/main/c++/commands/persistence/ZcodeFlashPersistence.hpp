/*
 * ZcodeFlashPersistence.hpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_ZCODEMBEDFLASHPERSISTENCE_HPP_
#define SRC_TEST_CPP_COMMANDS_ZCODEMBEDFLASHPERSISTENCE_HPP_

#include <ZcodeIncludes.hpp>
#include <ZcodeParameters.hpp>
#include "../../LowLevel/PersistenceLowLevel/FlashPage.hpp"

class ZcodeFlashPersistence {
    private:
        FlashPage page1;
        FlashPage page2;

        FlashPage* getCurrentValid();

        uint8_t incWithRoll(uint8_t val) {
            uint8_t newVal = val + 1;
            if (newVal == 0 || newVal == 0xFF) {
                return 1;
            }
            return newVal;
        }

    public:
        ZcodeFlashPersistence();

        bool hasGuid();

        bool hasMac();

        uint8_t* getGuid();

        uint8_t* getMac();

        uint8_t* getPersistentMemory();

        void writeGuid(const uint8_t *guid);

        void writeMac(const uint8_t *mac);

        void writePersistent(uint8_t location, const uint8_t *memory, uint8_t length);

    private:
        void writePersistentInternal(uint16_t location, const uint8_t *toWrite, uint16_t length);
//    int rewriteSector(uint16_t start, const uint8_t *toWrite);

};

#endif /* SRC_TEST_CPP_COMMANDS_ZCODEMBEDFLASHPERSISTENCE_HPP_ */

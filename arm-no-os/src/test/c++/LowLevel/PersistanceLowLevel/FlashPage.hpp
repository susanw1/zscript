/*
 * FlashPage.hpp
 *
 *  Created on: 3 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_PERSISTANCELOWLEVEL_FLASHPAGE_HPP_
#define SRC_TEST_CPP_LOWLEVEL_PERSISTANCELOWLEVEL_FLASHPAGE_HPP_
#include <string.h>
#include "../GeneralLLSetup.hpp"

class FlashPage {
    uint16_t pageNum;
    volatile flashProgramming_t *start;

    void erase();

    void unlockFlashWrite();
    void lockFlashWrite();

    void beginProgram();
    void endProgram();

    bool isBusy();

public:
    FlashPage(uint16_t pageNum);

    void zeroProgrammingSection(uint32_t offset) {
        unlockFlashWrite();
        beginProgram();
        while (isBusy())
            ;
        *(start + offset) = 0;
        while (isBusy())
            ;
        endProgram();
        lockFlashWrite();
    }

    const volatile flashProgramming_t* getStart() {
        return start;
    }

    uint32_t getLength() {
        return GeneralHalSetup::pageSize;
    }
    void eraseNoProgram() {
        unlockFlashWrite();
        while (isBusy())
            ;
        erase();
        while (isBusy())
            ;
        lockFlashWrite();
    }
    void programNoErase(flashProgramming_t *newContent, uint32_t newContentStartOffset, uint32_t newContentLength) {
        unlockFlashWrite();
        while (isBusy())
            ;
        beginProgram();
        for (uint32_t i = 0; i < newContentLength; ++i) {
            while (isBusy())
                ;
            start[newContentStartOffset + i] = newContent[i];
        }
        while (isBusy())
            ;
        endProgram();
        lockFlashWrite();
    }
    void overwriteWith(FlashPage *other, flashProgramming_t firstWord, const uint8_t *newContent, uint32_t newContentStartOffset, uint32_t newContentLength) {
        flashProgramming_t data[GeneralHalSetup::pageSize / sizeof(flashProgramming_t)];
        data[0] = firstWord;
        uint8_t *data8 = (uint8_t*) data;
        for (uint32_t i = sizeof(flashProgramming_t); i < GeneralHalSetup::pageSize; ++i) {
            if (i < newContentStartOffset || i >= newContentStartOffset + newContentLength) {
                data8[i] = ((uint8_t*) other->start)[i];
            } else {
                data8[i] = ((uint8_t*) newContent)[i - newContentStartOffset];
            }
        }
        unlockFlashWrite();
        while (isBusy())
            ;
        erase();
        while (isBusy())
            ;
        beginProgram();
        for (uint32_t i = 0; i < GeneralHalSetup::pageSize / sizeof(flashProgramming_t); ++i) {
            while (isBusy())
                ;
            start[i] = data[i];
        }
        while (isBusy())
            ;
        endProgram();
        lockFlashWrite();
    }
};
#endif /* SRC_TEST_CPP_LOWLEVEL_PERSISTANCELOWLEVEL_FLASHPAGE_HPP_ */

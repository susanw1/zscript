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
    flashProgramming_t *start;

    void erase();

    void unlockFlashWrite();
    void lockFlashWrite();

    bool isBusy();

public:
    FlashPage(uint16_t pageNum);

    void zeroProgrammingSection(flashProgramming_t *area) {
        *area = 0;
    }

    flashProgramming_t* getStart() {
        return start;
    }

    uint32_t getLength() {
        return GeneralHalSetup::pageSize;
    }

    void overwriteWith(FlashPage *other, uint8_t *newContent, uint32_t newContentStartOffset, uint32_t newContentLength) {
        unlockFlashWrite();
        while (isBusy())
            ;
        erase();
        flashProgramming_t data[GeneralHalSetup::pageSize / sizeof(flashProgramming_t)];
        uint8_t *data8 = (uint8_t*) data;
        for (uint32_t i = 0; i < GeneralHalSetup::pageSize; ++i) {
            if (i < newContentStartOffset || i >= newContentStartOffset + newContentLength) {
                data8[i] = ((uint8_t*) other->start)[i];
            } else {
                data8[i] = ((uint8_t*) newContent)[i - newContentStartOffset];
            }
        }
        for (uint32_t i = 0; i < GeneralHalSetup::pageSize / sizeof(flashProgramming_t); ++i) {
            while (isBusy())
                ;
            start[i] = data[i];
        }
        while (isBusy())
            ;
        lockFlashWrite();
    }
};
#endif /* SRC_TEST_CPP_LOWLEVEL_PERSISTANCELOWLEVEL_FLASHPAGE_HPP_ */

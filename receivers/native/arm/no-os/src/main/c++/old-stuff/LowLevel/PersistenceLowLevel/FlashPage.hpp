/*
 * FlashPage.hpp
 *
 *  Created on: 3 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_PERSISTANCELOWLEVEL_FLASHPAGE_HPP_
#define SRC_TEST_CPP_LOWLEVEL_PERSISTANCELOWLEVEL_FLASHPAGE_HPP_

#include <LowLevel/llIncludes.hpp>

template<class LL>
class FlashPage {
private:
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

        while (isBusy()) {
        }

        *(start + offset) = 0;

        while (isBusy()) {
        }

        endProgram();
        lockFlashWrite();
    }

    const volatile flashProgramming_t* getStart() {
        return start;
    }

    uint32_t getLength() {
        return LL::pageSize;
    }

    void eraseNoProgram() {
        unlockFlashWrite();
        while (isBusy()) {
        }

        erase();

        while (isBusy()) {
        }

        lockFlashWrite();
    }

    void programNoErase(flashProgramming_t *newContent, uint32_t newContentStartOffset, uint32_t newContentLength) {
        unlockFlashWrite();

        while (isBusy()) {
        }

        beginProgram();
        for (uint32_t i = 0; i < newContentLength; ++i) {
            while (isBusy())
                ;
            start[newContentStartOffset + i] = newContent[i];
        }

        while (isBusy()) {
        }

        endProgram();
        lockFlashWrite();
    }

    void overwriteWith(FlashPage *other, flashProgramming_t firstWord, const uint8_t *newContent, uint32_t newContentStartOffset, uint32_t newContentLength) {
        flashProgramming_t data[LL::pageSize / sizeof(flashProgramming_t)];
        data[0] = firstWord;
        uint8_t *data8 = (uint8_t*) data;
        for (uint32_t i = sizeof(flashProgramming_t); i < LL::pageSize; ++i) {
            if (i < newContentStartOffset || i >= newContentStartOffset + newContentLength) {
                data8[i] = ((uint8_t*) other->start)[i];
            } else {
                data8[i] = ((uint8_t*) newContent)[i - newContentStartOffset];
            }
        }
        unlockFlashWrite();

        while (isBusy()) {
        }

        erase();

        while (isBusy()) {
        }

        beginProgram();
        for (uint32_t i = 0; i < LL::pageSize / sizeof(flashProgramming_t); ++i) {
            while (isBusy()) {
            }

            start[i] = data[i];
        }

        while (isBusy()) {
        }

        endProgram();
        lockFlashWrite();
    }
};

template<class LL>
FlashPage<LL>::FlashPage(uint16_t pageNum) :
        pageNum(pageNum), start((flashProgramming_t*) (pageNum * LL::pageSize + 0x08000000)) {
}

template<class LL>
void FlashPage<LL>::unlockFlashWrite() {
    // magic keys...
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
}

template<class LL>
void FlashPage<LL>::lockFlashWrite() {
    // magic keys...
    const uint32_t lock = 0x80000000;
    const uint32_t lockOptions = 0x40000000;
    FLASH->CR = lock | lockOptions;
}

template<class LL>
void FlashPage<LL>::beginProgram() {
    const uint32_t clearAllStatus = 0x0001C3FB;
    const uint32_t enableProgram = 0x1;

    FLASH->SR |= clearAllStatus;
    FLASH->CR = enableProgram;
}

template<class LL>
void FlashPage<LL>::endProgram() {
    FLASH->CR = 0;
}

template<class LL>
void FlashPage<LL>::erase() {
    const uint32_t clearAllStatus = 0x0001C3FB;
    const uint32_t enablePageErase = 0x2;
    const uint32_t bank2Erase = 0x800;
    const uint32_t start = 0x10000;

    FLASH->SR |= clearAllStatus;
    if (pageNum > 127) {
        FLASH->CR = ((pageNum & 0x7F) << 3) | bank2Erase | enablePageErase; // use bank 2
    } else {
        FLASH->CR = (pageNum << 3) | enablePageErase;
    }
    FLASH->CR |= start;
}

template<class LL>
bool FlashPage<LL>::isBusy() {
    const uint32_t start = 0x10000;
    return (FLASH->SR &= start) != 0;
}
#endif /* SRC_TEST_CPP_LOWLEVEL_PERSISTANCELOWLEVEL_FLASHPAGE_HPP_ */
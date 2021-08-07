/*
 * FlashPage.cpp
 *
 *  Created on: 3 Jan 2021
 *      Author: robert
 */
#include "FlashPage.hpp"

FlashPage::FlashPage(uint16_t pageNum) :
        pageNum(pageNum), start((flashProgramming_t*) (pageNum * GeneralHalSetup::pageSize + 0x08000000)) {
}
void FlashPage::unlockFlashWrite() {
    // magic keys...
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
}

void FlashPage::lockFlashWrite() {
    // magic keys...
    const uint32_t lock = 0x80000000;
    const uint32_t lockOptions = 0x40000000;
    FLASH->CR = lock | lockOptions;
}
void FlashPage::beginProgram() {
    const uint32_t clearAllStatus = 0x0001C3FB;
    const uint32_t enableProgram = 0x1;

    FLASH->SR |= clearAllStatus;
    FLASH->CR = enableProgram;
}

void FlashPage::endProgram() {
    FLASH->CR = 0;
}

void FlashPage::erase() {
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

bool FlashPage::isBusy() {
    const uint32_t start = 0x10000;
    return (FLASH->SR &= start) != 0;
}

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
    FLASH->KEYR = 0x45670123;
    FLASH->KEYR = 0xCDEF89AB;
}

void FlashPage::lockFlashWrite() {
    FLASH->CR = 0xC0000000;
}
void FlashPage::beginProgram() {
    FLASH->SR |= 0xFFFFFFFF;
    FLASH->CR = 0x1;
}

void FlashPage::endProgram() {
    FLASH->CR = 0;
}

void FlashPage::erase() {
    FLASH->SR |= 0xFFFFFFFF;
    if (pageNum > 127) {
        FLASH->CR = ((pageNum & 0x7F) << 3) | 0x802; // use bank 2
    } else {
        FLASH->CR = (pageNum << 3) | 0x02;
    }
    FLASH->CR |= 0x10000;
}

bool FlashPage::isBusy() {
    return (FLASH->SR &= 0x10000) != 0;
}

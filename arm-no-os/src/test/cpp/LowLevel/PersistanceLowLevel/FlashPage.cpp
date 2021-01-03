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
    FLASH->CR |= 0x80000000;
}
void FlashPage::erase() {
    if (pageNum > 127) {
        FLASH->CR |= ((pageNum & 0x7F) << 3) | 0x804; // use bank 2
    } else {
        FLASH->CR |= (pageNum << 3) | 0x04;
    }
    FLASH->SR |= 0x10000;
}

bool FlashPage::isBusy() {
    return (FLASH->SR &= 0x10000) != 0;
}

/*
 * RCodeFlashPersistence.cpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#include "RCodeFlashPersistence.hpp"

static const uint16_t guidLocation = 16;
static const uint16_t macAddressLocation = 40;
static const uint16_t mainPersistentMemoryLocation = 48;

RCodeFlashPersistence::RCodeFlashPersistence() :
        page1(254), page2(255) {
}
FlashPage* RCodeFlashPersistence::getCurrentValid() {
    uint8_t p1_n = ((uint8_t) * page1.getStart());
    uint8_t p2_n = ((uint8_t) * page2.getStart());
    if ((p1_n == 0xFF && p2_n == 0xFF) || (p1_n == 0 && p2_n == 0)) {
        // some kind of invalid state - erase and start again
        flashProgramming_t newN = 1;
        page1.eraseNoProgram();
        page2.eraseNoProgram();
        page1.programNoErase(&newN, 0, 1);
        return &page1;
    } else if (p1_n == 0 || p1_n == 0xFF) {
        // p1 is the old one (might have erased but not overwriten)
        return &page2;
    } else if (p2_n == 0 || p2_n == 0xFF) {
        // p2 is the old one (might have erased but not overwriten)
        return &page1;
    } else if (p1_n == incWithRoll(p2_n)) {
        // p1 is the old one, now need to erase
        page1.eraseNoProgram();
        return &page2;
    } else if (p2_n == incWithRoll(p1_n)) {
        // p2 is the old one, now need to erase
        page2.eraseNoProgram();
        return &page1;
    } else {
        // the values are wrong somehow, e.g. died mid zeroing of old value
        flashProgramming_t newN = 1;
        page1.eraseNoProgram();
        page2.eraseNoProgram();
        page1.programNoErase(&newN, 0, 1);
        return &page1;
    }
}
bool RCodeFlashPersistence::hasGuid() {
    return *(((uint8_t*) (getCurrentValid()->getStart())) + guidLocation) == 0xAA;
}
bool RCodeFlashPersistence::hasMac() {
    return *(((uint8_t*) (getCurrentValid()->getStart())) + macAddressLocation) == 0xAA;
}
uint8_t* RCodeFlashPersistence::getGuid() {
    return ((uint8_t*) (getCurrentValid()->getStart())) + guidLocation + 1;
}
uint8_t* RCodeFlashPersistence::getMac() {
    return ((uint8_t*) (getCurrentValid()->getStart())) + macAddressLocation + 1;
}
uint8_t* RCodeFlashPersistence::getPersistentMemory() {
    return ((uint8_t*) (getCurrentValid()->getStart())) + mainPersistentMemoryLocation;
}
void RCodeFlashPersistence::writeGuid(const uint8_t *guid) {
    uint8_t guidWithMarker[17];
    for (int i = 0; i < 16; ++i) {
        guidWithMarker[i + 1] = guid[i];
    }
    guidWithMarker[0] = 0xAA;
    writePersistentInternal(guidLocation, guidWithMarker, 17);
}
void RCodeFlashPersistence::writeMac(const uint8_t *mac) {
    uint8_t macWithMarker[7];
    for (int i = 0; i < 6; ++i) {
        macWithMarker[i + 1] = mac[i];
    }
    macWithMarker[0] = 0xAA;
    writePersistentInternal(macAddressLocation, macWithMarker, 7);
}
void RCodeFlashPersistence::writePersistent(uint8_t location,
        const uint8_t *memory, uint8_t length) {
    writePersistentInternal(location + mainPersistentMemoryLocation,
            memory, length);
}
void RCodeFlashPersistence::writePersistentInternal(uint16_t location,
        const uint8_t *toWrite, uint16_t length) {
    FlashPage *oldPage = getCurrentValid();
    FlashPage *newPage;
    if (oldPage == &page1) {
        newPage = &page2;
    } else {
        newPage = &page1;
    }
    newPage->overwriteWith(oldPage, incWithRoll((uint8_t) * oldPage->getStart()), toWrite, location, length);
    oldPage->zeroProgrammingSection(0);
}

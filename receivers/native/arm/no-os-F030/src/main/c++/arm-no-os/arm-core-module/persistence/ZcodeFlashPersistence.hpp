/*
 * ZcodeFlashPersistence.hpp
 *
 *  Created on: 11 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_ZCODEMBEDFLASHPERSISTENCE_HPP_
#define SRC_TEST_CPP_COMMANDS_ZCODEMBEDFLASHPERSISTENCE_HPP_

#include <zcode/ZcodeIncludes.hpp>
#include <arm-no-os/arm-core-module/persistence/FlashPage.hpp>

template<class LL>
class ZcodeFlashPersistence {
private:
    FlashPage<LL> page1;
    FlashPage<LL> page2;

    FlashPage<LL>* getCurrentValid() {
        uint8_t p1_n = ((uint8_t) *page1.getStart());
        uint8_t p2_n = ((uint8_t) *page2.getStart());
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

    uint8_t incWithRoll(uint8_t val) {
        uint8_t newVal = val + 1;
        if (newVal == 0 || newVal == 0xFF) {
            return 1;
        }
        return newVal;
    }

public:

    static ZcodeFlashPersistence<LL> persist;

    ZcodeFlashPersistence() :
            page1(254), page2(255) {
    }

    bool hasGuid() {
        return *(((uint8_t*) (getCurrentValid()->getStart())) + LL::persistentGuidLocation) == 0xAA;
    }

    bool hasMac() {
        return *(((uint8_t*) (getCurrentValid()->getStart())) + LL::persistentMacAddressLocation) == 0xAA;
    }

    uint8_t* getGuid() {
        return ((uint8_t*) (getCurrentValid()->getStart())) + LL::persistentGuidLocation + 1;
    }

    uint8_t* getMac() {
        return ((uint8_t*) (getCurrentValid()->getStart())) + LL::persistentMacAddressLocation + 1;
    }

    uint8_t* getPersistentMemory() {
        return ((uint8_t*) (getCurrentValid()->getStart())) + LL::mainPersistentMemoryLocation;
    }

    void writeGuid(const uint8_t *guid) {
        uint8_t guidWithMarker[17];
        for (int i = 0; i < 16; ++i) {
            guidWithMarker[i + 1] = guid[i];
        }
        guidWithMarker[0] = 0xAA;
        writePersistentInternal(LL::persistentGuidLocation, guidWithMarker, 17);
    }

    void writeMac(const uint8_t *mac) {
        uint8_t macWithMarker[7];
        for (int i = 0; i < 6; ++i) {
            macWithMarker[i + 1] = mac[i];
        }
        macWithMarker[0] = 0xAA;
        writePersistentInternal(LL::persistentMacAddressLocation, macWithMarker, 7);
    }

    void writePersistent(uint8_t location, const uint8_t *memory, uint8_t length) {
        writePersistentInternal(location + LL::mainPersistentMemoryLocation,
                memory, length);
    }

private:
    void writePersistentInternal(uint16_t location, const uint8_t *toWrite, uint16_t length) {
        FlashPage<LL> *oldPage = getCurrentValid();
        FlashPage<LL> *newPage;
        if (oldPage == &page1) {
            newPage = &page2;
        } else {
            newPage = &page1;
        }
        newPage->overwriteWith(oldPage, incWithRoll((uint8_t) *oldPage->getStart()), toWrite, location, length);
        oldPage->zeroProgrammingSection(0);
    }

//    int rewriteSector(uint16_t start, const uint8_t *toWrite);

};

template<class LL>
ZcodeFlashPersistence<LL> ZcodeFlashPersistence<LL>::persist;

#endif /* SRC_TEST_CPP_COMMANDS_ZCODEMBEDFLASHPERSISTENCE_HPP_ */

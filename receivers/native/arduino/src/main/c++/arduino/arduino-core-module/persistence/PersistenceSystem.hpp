/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_PERSISTENCESYSTEM_HPP_
#define SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_PERSISTENCESYSTEM_HPP_

#include <EEPROM.h>

namespace Zscript {
template<class ZP>
class PersistenceSystem {
    static uint8_t currentOffset;
    static uint8_t notifChannelIdOffset;
    static uint8_t notifChannelDataOffset;

public:
    static void reserveNotifChannelData(uint8_t length) {
        notifChannelDataOffset = reserveSection(length);
    }

    static uint8_t reserveSection(uint8_t sectionLength) {
        uint8_t tmp = currentOffset;
        currentOffset += sectionLength + 2;
        return tmp;
    }

    static void writeSection(uint8_t offset, uint8_t sectionLength, uint8_t *data) {
        uint8_t num = EEPROM.read(offset);
        num++;
        if (num == 0xFF || num == 0) {
            num = 1;
        }
        EEPROM.update(offset, num);
        for (uint8_t i = 0; i < sectionLength; i++) {
            EEPROM.update(offset + i + 1, data[i]);
        }
        EEPROM.update(offset + sectionLength + 1, 0xFF - num);
    }

    static bool readSection(uint8_t offset, uint8_t sectionLength, uint8_t *destination) {
        uint8_t num = EEPROM.read(offset);
        if (num == 0 || num == 0xFF) {
            return false;
        }
        if(EEPROM.read(offset + sectionLength + 1) + num != 0xFF){
            return false;
        }
        for (uint8_t i = 0; i < sectionLength; i++) {
            destination[i] = EEPROM.read(offset + i + 1);
        }
        return true;
    }

    static uint8_t getNotifChannelIdOffset() {
        return notifChannelIdOffset;
    }

    static uint8_t getNotifChannelDataOffset() {
        return notifChannelDataOffset;
    }
};

template<class ZP>
uint8_t PersistenceSystem<ZP>::currentOffset = 0;

template<class ZP>
uint8_t PersistenceSystem<ZP>::notifChannelDataOffset = 0;

template<class ZP>
uint8_t PersistenceSystem<ZP>::notifChannelIdOffset = PersistenceSystem<ZP>::reserveSection(1);

}

#endif //SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_PERSISTENCESYSTEM_HPP_

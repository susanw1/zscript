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
    uint8_t currentOffset;

public:
    uint8_t reserveSection(uint8_t sectionLength) {
        uint8_t tmp = currentOffset;
        currentOffset += sectionLength + 2;
        return tmp;
    }

    void writeSection(uint8_t offset, uint8_t sectionLength, uint8_t *data) {
        uint8_t num = EEPROM.read(offset);
        num++;
        if (num == 0xFF) {
            num = 1;
        }
        EEPROM.update(offset, num);
        for (uint8_t i = 0; i < sectionLength; i++) {
            EEPROM.update(offset + i + 1, data[i]);
        }
        EEPROM.update(offset + sectionLength + 1, 0xFF - num);
    }

    bool readSection(uint8_t offset, uint8_t sectionLength, uint8_t *destination) {
        uint8_t num = EEPROM.read(offset);
        if (num == 0 || num == 0xFF) {
            return false;
        }
        for (uint8_t i = 0; i < sectionLength; i++) {
            destination[i] = EEPROM.read(offset + i + 1);
        }
        return EEPROM.read(offset + sectionLength + 1) + num == 0xFF;
    }
};
}
#endif //SRC_MAIN_CPP_ARDUINO_ARDUINO_CORE_MODULE_COMMANDS_PERSISTENCESYSTEM_HPP_

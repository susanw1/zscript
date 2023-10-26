/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F030X8_LLINCLUDES_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F030X8_LLINCLUDES_HPP_

#include <stdint.h>
#include <stdlib.h>

class HardwareDefinition {
public:
    static const uint32_t ucpdTxSOP = 0b10001110001100011000; // 3*Sync-1, Sync-2 = SOP

    static const uint8_t pinCount = 55;

    static const uint8_t atoDCount = 5;

    static const I2cIdentifier i2cCount = 2;
    static const SerialIdentifier uartCount = 2;
    static const SerialIdentifier serialCount = 2;

    static const DmaIdentifier dmaCount = 5;

    static const uint32_t pageSize = 0x400;
    static const uint16_t flashPageCount = 64;

    static const uint8_t spiAlternateFunction = 0;
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F030X8_LLINCLUDES_HPP_ */

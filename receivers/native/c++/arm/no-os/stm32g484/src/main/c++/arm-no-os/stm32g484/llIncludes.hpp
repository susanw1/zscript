/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G484_LLINCLUDES_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G484_LLINCLUDES_HPP_

#include <stdint.h>
#include <stdlib.h>

class HardwareDefinition {
public:
    static const uint32_t ucpdTxSOP = 0b10001110001100011000; // 3*Sync-1, Sync-2 = SOP

    static const uint8_t pinCount = 52;

    static const uint8_t atoDCount = 5;

    static const I2cIdentifier i2cCount = 4;
    static const SerialIdentifier uartCount = 6;
    static const SerialIdentifier serialCount = 7;
    static const DmaIdentifier dmaCount = 16;

    static const bool inDualBankFlash = true;
    static const uint32_t pageSize = 0x800;
    static const uint16_t flashPageCount = 256;

    static const uint8_t spiAlternateFunction = 5;
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G484_LLINCLUDES_HPP_ */

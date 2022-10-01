/*
 * llIncludes.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDESINDIVIDUAL_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDESINDIVIDUAL_HPP_

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

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDESINDIVIDUAL_HPP_ */

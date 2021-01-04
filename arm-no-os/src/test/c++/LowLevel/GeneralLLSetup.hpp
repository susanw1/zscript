/*
 * GeneralHalSetup.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_
#include <stdint.h>
#include <stdlib.h>
#include "stm32g4xx.h"
#include "stm32g484xx.h"

typedef uint8_t DmaIdentifier;
typedef uint8_t I2cIdentifier;
typedef uint8_t PinAlternateFunction;

typedef uint64_t flashProgramming_t;

#define I2C_1_SDA PB_9_
#define I2C_1_SCL PA_15_
#define USE_I2C_1

#define I2C_2_SDA PA_8_
#define I2C_2_SCL PC_4_
#define USE_I2C_2

#define I2C_3_SDA PC_9_
#define I2C_3_SCL PC_8_
#define USE_I2C_3

#define I2C_4_SDA PC_7_
#define I2C_4_SCL PC_6_
#define USE_I2C_4

class GeneralHalSetup {
public:
    static const uint8_t pinCount = 52;

    static const uint8_t systemClockCount = 13;

    static const I2cIdentifier i2cCount = 4;
    static const DmaIdentifier dmaCount = 16;

    static const DmaIdentifier i2c1Dma = 0;
    static const DmaIdentifier i2c2Dma = 1;
    static const DmaIdentifier i2c3Dma = 2;
    static const DmaIdentifier i2c4Dma = 3;

    static const bool inDualBankFlash = true;
    static const uint32_t pageSize = 0x800;
};

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_ */

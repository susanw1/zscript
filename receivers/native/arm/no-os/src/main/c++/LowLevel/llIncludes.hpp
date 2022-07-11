/*
 * llIncludes.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDES_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDES_HPP_

#include <stdint.h>
#include <stdlib.h>
#include "stm32g4xx.h"
#include "stm32g484xx.h"

typedef uint8_t DmaIdentifier;
typedef uint8_t SerialIdentifier;
typedef uint8_t I2cIdentifier;
typedef uint8_t PinAlternateFunction;

typedef uint64_t flashProgramming_t;

enum PeripheralOperationMode {
    DMA, INTERRUPT, SYNCHRONOUS
};

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDES_HPP_ */

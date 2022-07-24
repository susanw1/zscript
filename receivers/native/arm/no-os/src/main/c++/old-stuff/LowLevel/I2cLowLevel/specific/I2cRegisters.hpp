/*
 * I2cRegisters.hpp
 *
 *  Created on: 19 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CREGISTERS_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CREGISTERS_HPP_

#include <LowLevel/llIncludes.hpp>

struct I2cRegisters {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t OAR1;
    volatile uint32_t OAR2;
    volatile uint32_t TIMINGR;
    volatile uint32_t TIMEOUTR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t PECR;
    volatile uint32_t RXDR;
    volatile uint32_t TXDR;
};

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_SPECIFIC_I2CREGISTERS_HPP_ */

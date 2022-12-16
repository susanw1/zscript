/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CREGISTERS_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CREGISTERS_HPP_

#include <llIncludes.hpp>

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

#endif /* SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_LOWLEVEL_SPECIFIC_I2CREGISTERS_HPP_ */

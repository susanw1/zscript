/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_I2C_MODULE_ZCODEI2CMODULE_HPP_
#define SRC_MAIN_CPP_I2C_MODULE_ZCODEI2CMODULE_HPP_

#if defined(I2C_LL)
#include <i2c-ll/ZcodeI2cModule.hpp>

#else
#error No I2C system specified for usage
#endif

#endif /* SRC_MAIN_CPP_I2C_MODULE_ZCODEI2CMODULE_HPP_ */

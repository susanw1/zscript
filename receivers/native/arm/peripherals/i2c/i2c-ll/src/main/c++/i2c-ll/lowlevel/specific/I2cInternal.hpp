/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(I2C_LL_STM32G)
#include <i2c-ll-stm32f/lowlevel/specific/I2cInternal.hpp>
#elif defined(I2C_LL_STM32F)
#include <i2c-ll-stm32g/lowlevel/specific/I2cInternal.hpp>
#else
#error Please select a supported device family
#endif
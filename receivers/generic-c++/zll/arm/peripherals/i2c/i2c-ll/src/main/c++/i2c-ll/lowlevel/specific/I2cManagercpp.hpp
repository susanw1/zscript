/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include "../I2cManager.hpp"

#if defined(I2C_LL_STM32_Ver_2_v1_1)
#include <i2c-ll-stm32-2-1-1/lowlevel/specific/I2cManagercpp.hpp>
#else
#error Please select a supported device family
#endif

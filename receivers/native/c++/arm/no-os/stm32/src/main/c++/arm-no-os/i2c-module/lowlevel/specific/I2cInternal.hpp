/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include "I2cRegisters.hpp"
#if defined(STM32G4)
#include <arm-no-os/stm32g4/i2c-module/lowlevel/specific/I2cInternal.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/i2c-module/lowlevel/specific/I2cInternal.hpp>
#else
#error Please select a supported device family
#endif

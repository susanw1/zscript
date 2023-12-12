/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */
#ifndef DMA_LOWLEVEL
#error I2C DMA system requires DMA loading first. Please include an appropriate DMA system
#endif
#ifndef GPIO_LOWLEVEL
#error I2C system requires GPIO loading first. Please include an appropriate GPIO system
#endif
#ifndef LOWLEVEL_INCLUDE_DEVICE_SETUP
#error Device setup file not included. Please ensure an appropriate device file is loaded first, or #define LOWLEVEL_INCLUDE_DEVICE_SETUP to ignore.
#endif
#ifndef I2C_LL_STM32_Ver_2v1
#error I2C low level not compatible with device. Please ensure this is the correct I2C system, or #define I2C_LL_STM32_Ver_2v1 to ignore.
#endif

#ifndef IGNORE_PERIPHERAL_PIN_CHECKS
#include <zscript/stm32/devices/peripheralPinCheck.h>
#endif

#include <stdint.h>
#include <zscript/stm32/i2c/Ver_2v1/dma/I2c.hpp>
#include <zscript/stm32/i2c/Ver_2v1/dma/I2c.hpp>

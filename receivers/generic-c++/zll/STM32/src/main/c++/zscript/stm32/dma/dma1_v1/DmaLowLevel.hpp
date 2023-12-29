/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifdef DMA_LOWLEVEL
#error DMA system must be included only once per cpp file (and only one type of DMA system may be included)
#endif

#define DMA_LOWLEVEL

#ifndef LOWLEVEL_INCLUDE_DEVICE_SETUP
#error Device setup file not included. Please ensure an appropriate device file is loaded first, or #define LOWLEVEL_INCLUDE_DEVICE_SETUP to ignore.
#endif
#ifndef I2C_LL_STM32_Ver_2v1
#error I2C low level not compatible with device. Please ensure this is the correct I2C system, or #define I2C_LL_STM32_Ver_2v1 to ignore.
#endif

#ifndef IGNORE_PERIPHERAL_PIN_CHECKS
#include <zscript/stm32/devices/peripheralPinCheck.h>
#endif

#include <utility/PreprocessorArrayInitialiser.hpp>
#include <zscript/stm32/i2c/Ver_2v1/common/I2cRegisters.hpp>
#include <zscript/stm32/i2c/Ver_2v1/common/I2cDefinitions.hpp>
#include <zscript/stm32/i2c/Ver_2v1/common/I2cInternal.hpp>
#include <zscript/stm32/i2c/Ver_2v1/dma/I2c.hpp>
#include <zscript/stm32/i2c/Ver_2v1/dma/I2ccpp.hpp>
#include <zscript/stm32/i2c/Ver_2v1/dma/I2cManager.hpp>
#include <zscript/stm32/i2c/Ver_2v1/dma/I2cManagercpp.hpp>

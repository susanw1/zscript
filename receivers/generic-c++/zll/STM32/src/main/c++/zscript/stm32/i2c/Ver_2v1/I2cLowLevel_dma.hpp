/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifdef I2C_LOWLEVEL
#error I2C system must be included only once per cpp file (and only one type of I2C system may be included)
#endif

#define I2C_LOWLEVEL

#ifndef DMA_LOWLEVEL
#error I2C DMA system requires DMA loading first. Please include an appropriate DMA system
#endif
#ifndef GPIO_LOWLEVEL
#error I2C system requires GPIO loading first. Please include an appropriate GPIO system
#endif
#ifndef LOWLEVEL_INCLUDE_DEVICE_SETUP
#error Device setup file not included. Please ensure an appropriate device file is loaded first, or #define LOWLEVEL_INCLUDE_DEVICE_SETUP to ignore.
#endif
#ifndef DMA_LL_STM32_Ver_1v1
#error DMA low level not compatible with device. Please ensure this is the correct DMA system, or #define DMA_LL_STM32_Ver_1v1 to ignore.
#endif

#include <zscript/stm32/dma/dma1_v1/impl/DmaMuxRequest.hpp>
#include <zscript/stm32/dma/dma1_v1/impl/DmaRegisters.hpp>
#include <zscript/stm32/dma/dma1_v1/impl/DmaChannelInternal.hpp>
#include <zscript/stm32/dma/dma1_v1/impl/Dma.hpp>
#include <zscript/stm32/dma/dma1_v1/impl/Dmacpp.hpp>
#include <zscript/stm32/dma/dma1_v1/impl/DmaManager.hpp>
#include <zscript/stm32/dma/dma1_v1/impl/DmaManagercpp.hpp>

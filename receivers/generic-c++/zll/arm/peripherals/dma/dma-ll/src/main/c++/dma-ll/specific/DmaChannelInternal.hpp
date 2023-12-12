/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(DMA_LL_STM32G)
#include <dma-ll-stm32g/specific/DmaChannelInternal.hpp>
#elif defined(DMA_LL_STM32F)
#include <dma-ll-stm32f/specific/DmaChannelInternal.hpp>
#else
#error Please select a supported device family
#endif

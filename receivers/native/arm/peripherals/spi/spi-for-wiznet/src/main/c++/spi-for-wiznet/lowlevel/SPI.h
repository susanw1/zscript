/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(SPI_WIZNET_STM32)
#include <spi-for-wiznet-stm32/lowlevel/SPI.h>
#else
#error Please select a supported device family
#endif

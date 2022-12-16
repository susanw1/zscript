/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(UART_LL_STM32G)
#include <uart-ll-stm32f/lowlevel/specific/UartInternal.hpp>
#elif defined(UART_LL_STM32F)
#include <uart-ll-stm32g/lowlevel/specific/UartInternal.hpp>
#else
#error Please select a supported device family
#endif

#include "../Uart.hpp"

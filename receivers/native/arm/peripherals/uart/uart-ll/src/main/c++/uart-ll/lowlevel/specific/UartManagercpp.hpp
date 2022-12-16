/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include "../UartManager.hpp"

#if defined(UART_LL_STM32G)
#include <uart-ll-stm32f/lowlevel/specific/UartManagercpp.hpp>
#elif defined(UART_LL_STM32F)
#include <uart-ll-stm32g/lowlevel/specific/UartManagercpp.hpp>
#else
#error Please select a supported device family
#endif

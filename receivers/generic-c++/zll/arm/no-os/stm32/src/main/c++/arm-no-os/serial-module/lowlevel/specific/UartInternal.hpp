/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include "UartRegisters.hpp"
#if defined(STM32G4)
#include <arm-no-os/stm32g4/serial-module/lowlevel/specific/UartInternal.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/serial-module/lowlevel/specific/UartInternal.hpp>
#else
#error Please select a supported device family
#endif

#include "../Uart.hpp"

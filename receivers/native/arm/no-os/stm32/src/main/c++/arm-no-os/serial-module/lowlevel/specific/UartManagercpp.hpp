/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include "../UartManager.hpp"
#if defined(STM32G4)
#include <arm-no-os/stm32g4/serial-module/lowlevel/specific/UartManagercpp.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/serial-module/lowlevel/specific/UartManagercpp.hpp>
#else
#error Please select a supported device family
#endif

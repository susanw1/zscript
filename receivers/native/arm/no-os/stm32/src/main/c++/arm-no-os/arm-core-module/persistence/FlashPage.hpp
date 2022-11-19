/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(STM32G4)
#include <arm-no-os/stm32g4/arm-core-module/persistence/FlashPage.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/arm-core-module/persistence/FlashPage.hpp>
#else
#error Please select a supported device family
#endif

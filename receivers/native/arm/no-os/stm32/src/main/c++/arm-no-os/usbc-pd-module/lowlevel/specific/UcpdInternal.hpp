/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(STM32G4)
#include <arm-no-os/stm32g4/usbc-pd-module/lowlevel/specific/UcpdInternal.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/usbc-pd-module/lowlevel/specific/UcpdInternal.hpp>
#else
#error Please select a supported device family
#endif

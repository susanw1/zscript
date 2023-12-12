/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#if defined(STM32)
#include <pins-ll-stm32/lowlevel/specific/Gpiocpp.hpp>
#else
#error Please select a supported device family
#endif

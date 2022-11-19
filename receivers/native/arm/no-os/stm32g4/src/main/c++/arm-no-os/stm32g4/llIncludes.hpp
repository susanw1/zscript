/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_LLINCLUDES_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_LLINCLUDES_HPP_

#include <stdint.h>
#include <stdlib.h>

#include "system/core/stm32g4xx.h"

typedef uint8_t DmaIdentifier;
typedef uint8_t SerialIdentifier;
typedef uint8_t I2cIdentifier;
typedef uint8_t PinAlternateFunction;

typedef uint64_t flashProgramming_t;

#if defined(STM32G431xx)
#include <arm-no-os/stm32g431/llIncludes.hpp>
#elif defined(STM32G441xx)
#include <arm-no-os/stm32g441/llIncludes.hpp>
#elif defined(STM32G471xx)
#include <arm-no-os/stm32g471/llIncludes.hpp>
#elif defined(STM32G473xx)
#include <arm-no-os/stm32g473/llIncludes.hpp>
#elif defined(STM32G483xx)
#include <arm-no-os/stm32g483/llIncludes.hpp>
#elif defined(STM32G474xx)
#include <arm-no-os/stm32g474/llIncludes.hpp>
#elif defined(STM32G484xx)
#include <arm-no-os/stm32g484/llIncludes.hpp>
#elif defined(STM32GBK1CB)
#include <arm-no-os/stm32gbk1cb/llIncludes.hpp>
#else
#error "Please select the target STM32G4xx device used in your application"
#endif

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_LLINCLUDES_HPP_ */

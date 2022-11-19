/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_LLINCLUDES_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_LLINCLUDES_HPP_

#if defined(STM32G431xx) or defined(STM32G441xx) or defined(STM32G471xx) \
    or defined(STM32G473xx) or defined(STM32G483xx) or defined(STM32G474xx) \
    or defined(STM32G484xx) or defined(STM32GBK1CB)
#define STM32G4
#elif defined(STM32F030x6) or defined(STM32F030x8) or defined(STM32F031x6) \
    or defined(STM32F038xx) or defined(STM32F042x6) or defined(STM32F048xx) \
    or defined(STM32F051x8) or defined(STM32F058xx) or defined(STM32F070x6) \
    or defined(STM32F070xB) or defined(STM32F071xB) or defined(STM32F072xB) \
    or defined(STM32F078xx) or defined(STM32F091xC) or defined(STM32F098xx) \
    or  defined(STM32F030xC)
#define STM32F0
#else
#error "Please select first the target device used in your application"
#endif

#if defined(STM32G4)
#include <arm-no-os/stm32g4/llIncludes.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/llIncludes.hpp>
#else
#error Please select a supported device family
#endif

enum PeripheralOperationMode {
    DMA, INTERRUPT, SYNCHRONOUS
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_LLINCLUDES_HPP_ */

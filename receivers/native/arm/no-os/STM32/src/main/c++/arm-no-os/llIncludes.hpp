/*
 * llIncludes.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDES_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDES_HPP_

#if defined(STM32G431xx) or defined(STM32G441xx) or defined(STM32G471xx) \
    or defined(STM32G473xx) or defined(STM32G483xx) or defined(STM32G474xx) \
    or defined(STM32G484xx) or defined(STM32GBK1CB)
#define STM32G4
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

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDES_HPP_ */

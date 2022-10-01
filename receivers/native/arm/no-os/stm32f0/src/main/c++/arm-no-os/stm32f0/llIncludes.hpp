/*
 * llIncludes.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDESSPECIFIC_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDESSPECIFIC_HPP_

#include <stdint.h>
#include <stdlib.h>

#include "system/core/stm32f0xx.h"

typedef uint8_t DmaIdentifier;
typedef uint8_t SerialIdentifier;
typedef uint8_t I2cIdentifier;
typedef uint8_t PinAlternateFunction;

typedef uint16_t flashProgramming_t;

#if defined(STM32F030x6)
#include <arm-no-os/stm32f030x6/llIncludes.hpp>
#elif defined(STM32F030x8)
#include <arm-no-os/stm32f030x8/llIncludes.hpp>
#elif defined(STM32F031x6)
#include <arm-no-os/stm32f031x6/llIncludes.hpp>
#elif defined(STM32F038xx)
#include <arm-no-os/stm32f038/llIncludes.hpp>
#elif defined(STM32F042x6)
#include <arm-no-os/stm32f042x6/llIncludes.hpp>
#elif defined(STM32F048xx)
#include <arm-no-os/stm32f048/llIncludes.hpp>
#elif defined(STM32F051x8)
#include <arm-no-os/stm32f051x8/llIncludes.hpp>
#elif defined(STM32F058xx)
#include <arm-no-os/stm32f058/llIncludes.hpp>
#elif defined(STM32F070x6)
#include <arm-no-os/stm32f070x6/llIncludes.hpp>
#elif defined(STM32F070xB)
#include <arm-no-os/stm32f070xb/llIncludes.hpp>
#elif defined(STM32F071xB)
#include <arm-no-os/stm32f071xb/llIncludes.hpp>
#elif defined(STM32F072xB)
#include <arm-no-os/stm32f072xb/llIncludes.hpp>
#elif defined(STM32F078xx)
#include <arm-no-os/stm32f078/llIncludes.hpp>
#elif defined(STM32F091xC)
#include <arm-no-os/stm32f091xc/llIncludes.hpp>
#elif defined(STM32F098xx)
#include <arm-no-os/stm32f098/llIncludes.hpp>
#elif defined(STM32F030xC)
#include <arm-no-os/stm32f030xc/llIncludes.hpp>
#else
#error "Please select the target STM32F0xx device used in your application"
#endif

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_LLINCLUDESSPECIFIC_HPP_ */

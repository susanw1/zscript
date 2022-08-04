/*
 * GpioNames.hpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */

#include <arm-no-os/llIncludes.hpp>

#if defined(STM32G4)
#include <arm-no-os/stm32g4/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/pins-module/lowlevel/specific/GpioNames.hpp>
#else
#error Please select a supported device family
#endif

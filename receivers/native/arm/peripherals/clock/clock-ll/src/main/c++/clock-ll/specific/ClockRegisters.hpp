/*
 * ClockRegisters.hpp
 *
 *  Created on: 23 Dec 2022
 *      Author: alicia
 */

#if defined(CLOCK_LL_STM32G)
#include <clock-ll-stm32g/specific/ClockRegisters.hpp>
#elif defined(CLOCK_LL_STM32F)
#include <clock-ll-stm32f/specific/ClockRegisters.hpp>
#else
#error Please select a supported device family
#endif

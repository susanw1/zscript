/*
 * Clock.hpp
 *
 *  Created on: 16 Dec 2022
 *      Author: alicia
 */

#if defined(CLOCK_LL_STM32G)
#include <clock-ll-stm32g/specific/Clock.hpp>
#elif defined(CLOCK_LL_STM32F)
#include <clock-ll-stm32f/specific/Clock.hpp>
#else
#error Please select a supported device family
#endif

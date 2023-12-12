/*
 * InterruptManager.hpp
 *
 *  Created on: 22 Dec 2022
 *      Author: alicia
 */

#if defined(INTERRUPT_LL_STM32G)
#include <interrupt-ll-stm32g/specific/InterruptManager.hpp>
#elif defined(INTERRUPT_LL_STM32F)
#include <interrupt-ll-stm32f/specific/InterruptManager.hpp>
#else
#error Please select a supported device family
#endif

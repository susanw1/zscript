/*
 * InterruptManager.hpp
 *
 *  Created on: 5 Jul 2022
 *      Author: robert
 */

#if defined(STM32G4)
#include <arm-no-os/stm32g4/system/interrupt/InterruptManager.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/system/interrupt/InterruptManager.hpp>
#else
#error Please select a supported device family
#endif

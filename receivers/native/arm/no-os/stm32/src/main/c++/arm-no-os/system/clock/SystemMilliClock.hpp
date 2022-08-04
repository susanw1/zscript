/*
 * SystemMilliClock.hpp
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#if defined(STM32G4)
#include <arm-no-os/stm32g4/system/clock/SystemMilliClock.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/system/clock/SystemMilliClock.hpp>
#else
#error Please select a supported device family
#endif

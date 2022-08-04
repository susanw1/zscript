/*
 * FlashPage.hpp
 *
 *  Created on: 3 Jan 2021
 *      Author: robert
 */

#if defined(STM32G4)
#include <arm-no-os/stm32g4/arm-core-module/persistence/FlashPage.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/arm-core-module/persistence/FlashPage.hpp>
#else
#error Please select a supported device family
#endif

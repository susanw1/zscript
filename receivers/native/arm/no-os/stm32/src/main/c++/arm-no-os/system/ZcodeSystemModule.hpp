/*
 * ZcodeSystemModule.hpp
 *
 *  Created on: 4 Aug 2022
 *      Author: robert
 */

#if defined(STM32G4)
#include <arm-no-os/stm32g4/system/ZcodeSystemModule.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/system/ZcodeSystemModule.hpp>
#else
#error Please select a supported device family
#endif

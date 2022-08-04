/*
 * ZcodeI2cBusInterruptSource.hpp
 *
 *  Created on: 4 Aug 2022
 *      Author: robert
 */

#if defined(STM32G4)
#include <arm-no-os/stm32g4/i2c-module/addressing/ZcodeI2cBusInterruptSource.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/i2c-module/addressing/ZcodeI2cBusInterruptSource.hpp>
#else
#error Please select a supported device family
#endif

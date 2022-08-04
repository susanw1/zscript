/*
 * I2cManagercpp.hpp
 *
 *  Created on: 4 Aug 2022
 *      Author: robert
 */

#include "../I2cManager.hpp"

#if defined(STM32G4)
#include <arm-no-os/stm32g4/i2c-module/lowlevel/specific/I2cManagercpp.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/i2c-module/lowlevel/specific/I2cManagercpp.hpp>
#else
#error Please select a supported device family
#endif

/*
 * GpioManagercpp.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */
#include "../GpioManager.hpp"

#if defined(STM32G4)
#include <arm-no-os/stm32g4/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#else
#error Please select a supported device family
#endif

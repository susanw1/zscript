/*
 * PinController.hpp
 *
 *  Created on: 25 Jul 2022
 *      Author: robert
 */

#if defined(STM32G4)
#include <arm-no-os/stm32g4/pins-module/pin-controller/PinController.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/pins-module/pin-controller/PinController.hpp>
#else
#error Please select a supported device family
#endif

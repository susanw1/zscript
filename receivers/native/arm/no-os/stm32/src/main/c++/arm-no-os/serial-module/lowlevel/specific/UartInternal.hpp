/*
 * UartInternal.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#include "UartRegisters.hpp"

#if defined(STM32G4)
#include <arm-no-os/stm32g4/serial-module/lowlevel/specific/UartInternal.hpp>
#elif defined(STM32F0)
#include <arm-no-os/stm32f0/serial-module/lowlevel/specific/UartInternal.hpp>
#else
#error Please select a supported device family
#endif

#include "../Uart.hpp"

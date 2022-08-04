/*
 * GpioManagercpp.hpp
 *
 *  Created on: 8 Jul 2022
 *      Author: robert
 */

#if defined(STM32G431xx)
#include <arm-no-os/stm32g431/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#elif defined(STM32G441xx)
#include <arm-no-os/stm32g441/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#elif defined(STM32G471xx)
#include <arm-no-os/stm32g471/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#elif defined(STM32G473xx)
#include <arm-no-os/stm32g473/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#elif defined(STM32G483xx)
#include <arm-no-os/stm32g483/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#elif defined(STM32G474xx)
#include <arm-no-os/stm32g474/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#elif defined(STM32G484xx)
#include <arm-no-os/stm32g484/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#elif defined(STM32GBK1CB)
#include <arm-no-os/stm32gbk1cb/pins-module/lowlevel/specific/GpioManagercpp.hpp>
#else
#error "Please select the target STM32G4xx device used in your application"
#endif

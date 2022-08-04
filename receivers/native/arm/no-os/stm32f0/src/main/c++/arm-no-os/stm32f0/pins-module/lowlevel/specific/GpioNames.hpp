/*
 * GpioNames.hpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */

#if defined(STM32F030x6)
#include <arm-no-os/stm32f030x6/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F030x8)
#include <arm-no-os/stm32f030x8/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F031x6)
#include <arm-no-os/stm32f031x6/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F038xx)
#include <arm-no-os/stm32f038/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F042x6)
#include <arm-no-os/stm32f042x6/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F048xx)
#include <arm-no-os/stm32f048/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F051x8)
#include <arm-no-os/stm32f051x8/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F058xx)
#include <arm-no-os/stm32f058/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F070x6)
#include <arm-no-os/stm32f070x6/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F070xB)
#include <arm-no-os/stm32f070xb/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F071xB)
#include <arm-no-os/stm32f071xb/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F072xB)
#include <arm-no-os/stm32f072xb/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F078xx)
#include <arm-no-os/stm32f078/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F091xC)
#include <arm-no-os/stm32f091xc/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F098xx)
#include <arm-no-os/stm32f098/pins-module/lowlevel/specific/GpioNames.hpp>
#elif defined(STM32F030xC)
#include <arm-no-os/stm32f030xc/pins-module/lowlevel/specific/GpioNames.hpp>
#else
#error "Please select the target STM32F0xx device used in your application"
#endif

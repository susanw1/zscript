/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifdef GPIO_LOWLEVEL
#error GPIO system must be included only once per cpp file (and only one type of GPIO system may be included)
#endif

#define GPIO_LOWLEVEL

#ifndef LOWLEVEL_INCLUDE_DEVICE_SETUP
#error Device setup file not included. Please ensure an appropriate device file is loaded first, or #define LOWLEVEL_INCLUDE_DEVICE_SETUP to ignore.
#endif

#ifndef IGNORE_PERIPHERAL_PIN_CHECKS
#include <zscript/stm32/devices/peripheralPinCheck.h>
#endif

#include <utility/PreprocessorArrayInitialiser.hpp>
#include <zscript/stm32/pins/gpio_v1/impl/GpioPort.hpp>
#include <zscript/stm32/pins/gpio_v1/impl/Gpio.hpp>
#include <zscript/stm32/pins/gpio_v1/impl/GpioManager.hpp>
#include <zscript/stm32/pins/gpio_v1/impl/Gpiocpp.hpp>
#include <zscript/stm32/pins/gpio_v1/impl/GpioManagercpp.hpp>

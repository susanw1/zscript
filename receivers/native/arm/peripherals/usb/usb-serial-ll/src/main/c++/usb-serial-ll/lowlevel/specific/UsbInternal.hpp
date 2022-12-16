/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <usb-serial-ll/UsbLLInterfaceInclude.hpp>

#if defined(USB_SERIAL_LL_STM32G)
#include <usb-serial-ll-stm32g4/lowlevel/specific/UsbInternal.hpp>
#else
#error Please select a supported device family
#endif

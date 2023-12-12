/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USB_MODULE_LOWLEVEL_SPECIFIC_USBREGISTERS_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USB_MODULE_LOWLEVEL_SPECIFIC_USBREGISTERS_HPP_

#include <llIncludes.hpp>
struct UsbRegisters {
    volatile uint32_t CNTR;
    volatile uint32_t ISTR;
    volatile uint32_t FNR;
    volatile uint32_t DADDR;
    volatile uint32_t BTABLE;
    volatile uint32_t LPMCSR;
    volatile uint32_t BCDR;
};

struct UsbEndpointRegisters {
    volatile uint32_t EP0R;
    volatile uint32_t EP1R;
    volatile uint32_t EP2R;
    volatile uint32_t EP3R;
    volatile uint32_t EP4R;
    volatile uint32_t EP5R;
    volatile uint32_t EP6R;
    volatile uint32_t EP7R;
};

struct UsbPbm {
    uint16_t buffer[512];

    uint16_t* getbufferDescriptor() {
        return buffer;
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USB_MODULE_LOWLEVEL_SPECIFIC_USBREGISTERS_HPP_ */

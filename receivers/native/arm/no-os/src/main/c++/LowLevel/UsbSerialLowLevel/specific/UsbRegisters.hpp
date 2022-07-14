/*
 * UsbRegisters.hpp
 *
 *  Created on: 24 Jul 2021
 *      Author: robert
 */

#ifndef LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBREGISTERS_HPP_
#define LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBREGISTERS_HPP_

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

#endif /* LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBREGISTERS_HPP_ */

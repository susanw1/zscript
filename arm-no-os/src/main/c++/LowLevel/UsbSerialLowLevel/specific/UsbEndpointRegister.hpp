/*
 * EndpointRegister.hpp
 *
 *  Created on: 28 Jul 2021
 *      Author: robert
 */

#ifndef LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBENDPOINTREGISTER_HPP_
#define LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBENDPOINTREGISTER_HPP_
#include "../../GeneralLLSetup.hpp"
#include "UsbRegisters.hpp"

enum UsbEndpointType {
    UsbBulkEndpoint = 0,
    UsbControlEndpoint = 1,
    UsbIsoEndpoint = 2,
    UsbInterruptEndpoint = 3,
};
enum UsbResponseStatus {
    UsbEndpointDisabled = 0,
    UsbEndpointStall = 1,
    UsbEndpointNak = 2,
    UsbEndpointValid = 3,
};

class UsbEndpointRegister {
    volatile uint16_t *reg;

    uint16_t generateNoOpValue() {
        uint16_t val = 0x8080;
        val |= (*reg) & 0x070F;
        return val;
    }
public:
    UsbEndpointRegister() :
            reg(NULL) {
    }
    UsbEndpointRegister(volatile uint16_t *reg) :
            reg(reg) {
    }
    void setEndpointAddress(uint8_t addr) {
        uint16_t val = generateNoOpValue() | (addr & 0x0F);
        *reg = val;
    }
    void setEndpointType(UsbEndpointType type) {
        uint16_t val = generateNoOpValue() | (type << 9);
        *reg = val;
    }
    void setEndpointRxStatus(UsbResponseStatus status) {
        uint16_t current = *reg;
        current &= 0x3000;
        current ^= status << 12;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void setEndpointTxStatus(UsbResponseStatus status) {
        uint16_t current = *reg;
        current &= 0x0030;
        current ^= status << 4;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void setEndpointKind(bool highNLow) {
        uint16_t val = generateNoOpValue();
        if (highNLow) {
            val |= 0x0100;
        } else {
            val &= ~0x0100;
        }
        *reg = val;
    }
    void clearRxComplete() {
        uint16_t val = generateNoOpValue() & ~0x8000;
        *reg = val;
    }
    void clearTxComplete() {
        uint16_t val = generateNoOpValue() & ~0x0080;
        *reg = val;
    }
    void clearRxDataToggle() {
        uint16_t current = *reg;
        current &= 0x0040;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void setRxDataToggle() {
        uint16_t current = *reg;
        current &= 0x0040;
        current ^= 0x0040;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void clearTxDataToggle() {
        uint16_t current = *reg;
        current &= 0x4000;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void setTxDataToggle() {
        uint16_t current = *reg;
        current &= 0x4000;
        current ^= 0x4000;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    bool getRxComplete() {
        return (*reg & 0x8000) != 0;
    }
    bool getTxComplete() {
        return (*reg & 0x0080) != 0;
    }
    bool getSetupLast() {
        return (*reg & 0x0800) != 0;
    }
    void resetAll() {
        uint16_t val = *reg;
        val ^= 0x0000;
        val &= 0x7070;
        *reg = val;
    }
};

#endif /* LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBENDPOINTREGISTER_HPP_ */

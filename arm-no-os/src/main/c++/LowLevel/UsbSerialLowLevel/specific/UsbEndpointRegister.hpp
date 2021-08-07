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
    static const uint16_t rxComplete = 0x8000;
    static const uint16_t rxDataToggle = 0x4000;
    static const uint16_t rxStat = 0x3000;
    static const uint16_t setupLast = 0x0800;
    static const uint16_t epType = 0x0600;
    static const uint16_t epKind = 0x0100;
    static const uint16_t txComplete = 0x0080;
    static const uint16_t txDataToggle = 0x0040;
    static const uint16_t txStat = 0x0030;
    static const uint16_t epAddr = 0x000F;

    volatile uint16_t *reg;

    uint16_t generateNoOpValue() {
        uint16_t val = rxComplete | txComplete;
        val |= (*reg) & (epType | epKind | epAddr);
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
        uint16_t val = (generateNoOpValue() & ~epAddr) | (addr & epAddr);
        *reg = val;
    }
    void setEndpointType(UsbEndpointType type) {
        uint16_t val = (generateNoOpValue() & ~epType) | (type << 9);
        *reg = val;
    }
    void setEndpointRxStatus(UsbResponseStatus status) {
        uint16_t current = *reg;
        current ^= status << 12;
        current &= rxStat;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void setEndpointTxStatus(UsbResponseStatus status) {
        uint16_t current = *reg;
        current ^= status << 4;
        current &= txStat;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void setEndpointKind(bool highNLow) {
        uint16_t val = generateNoOpValue();
        if (highNLow) {
            val |= epKind;
        } else {
            val &= ~epKind;
        }
        *reg = val;
    }
    void clearRxComplete() {
        uint16_t val = generateNoOpValue() & ~rxComplete;
        *reg = val;
    }
    void clearTxComplete() {
        uint16_t val = generateNoOpValue() & ~txComplete;
        *reg = val;
    }
    void clearRxDataToggle() {
        uint16_t current = *reg;
        current &= rxDataToggle;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void setRxDataToggle() {
        uint16_t current = *reg;
        current ^= rxDataToggle;
        current &= rxDataToggle;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void clearTxDataToggle() {
        uint16_t current = *reg;
        current &= txDataToggle;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    void setTxDataToggle() {
        uint16_t current = *reg;
        current ^= txDataToggle;
        current &= txDataToggle;
        uint16_t val = generateNoOpValue() | current;
        *reg = val;
    }
    bool getRxComplete() {
        return (*reg & rxComplete) != 0;
    }
    bool getTxComplete() {
        return (*reg & txComplete) != 0;
    }
    bool getSetupLast() {
        return (*reg & setupLast) != 0;
    }
    void resetAll() {
        uint16_t val = *reg;
        val ^= 0x0000;
        val &= (rxDataToggle | rxStat | txDataToggle | txStat);
        *reg = val;
    }
};

#endif /* LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBENDPOINTREGISTER_HPP_ */

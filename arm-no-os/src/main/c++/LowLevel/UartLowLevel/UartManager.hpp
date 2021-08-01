/*
 * UartManager.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_
#include "../GeneralLLSetup.hpp"
#include "Uart.hpp"
#include "../UsbSerialLowLevel/Usb.hpp"

class Serial;

class UartManager {
private:

    static Uart uarts[GeneralHalSetup::uartCount];

#ifdef USE_USB_SERIAL
    static Usb usb;

#endif

    UartManager() {
    }
    friend UartInterruptManager;

public:
    static void init();

    static Serial* getUartById(SerialIdentifier id) {
#ifdef USE_USB_SERIAL
        if (id == GeneralHalSetup::UsbSerialId) {
            return &usb;
        }
#endif
        return uarts + id;
    }
};

#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_ */

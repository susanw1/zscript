/*
 * UartManager.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_

#include <llIncludes.hpp>
#include "Uart.hpp"
#include <UsbSerialLowLevel/Usb.hpp>

class Serial;

template<class LL>
class UartManager {
private:
    static Uart<LL> uarts[LL::uartCount];

    static void interrupt(uint8_t id);

    UartManager() {
    }

public:
    static void init();

    static Serial* getUartById(SerialIdentifier id) {

#ifdef USE_USB_SERIAL
        if (id == GeneralHalSetup::UsbSerialId) {
            return &Usb<LL>::usb;
        }
#endif
        return uarts + id;
    }
};
#include "specific/UartManagercpp.hpp"

#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_ */

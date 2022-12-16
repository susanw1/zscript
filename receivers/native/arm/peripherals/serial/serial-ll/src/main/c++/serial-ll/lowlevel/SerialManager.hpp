/*
 * SerialManager.hpp
 *
 *  Created on: 16 Dec 2022
 *      Author: alicia
 */

#ifndef PERIPHERALS_SERIAL_SERIAL_LL_SRC_MAIN_C___SERIAL_LL_LOWLEVEL_SERIALMANAGER_HPP_
#define PERIPHERALS_SERIAL_SERIAL_LL_SRC_MAIN_C___SERIAL_LL_LOWLEVEL_SERIALMANAGER_HPP_

#include <serial-ll/SerialLLInterfaceInclude.hpp>

#ifdef UART_LL
#include <uart-ll/lowlevel/UartManager.hpp>
#endif
#ifdef USB_SERIAL_LL
#include <usb-ll/lowlevel/Usb.hpp>
#endif

#include <llIncludes.hpp>
#include "Serial.hpp"

class Serial;

template<class LL>
class SerialManager {
    typedef typename LL::HW HW;

private:

    static uint16_t mask;

    static void interrupt(uint8_t id);

    SerialManager() {
    }

public:
    static const SerialIdentifier serialCount = 0

#ifdef UART_LL
        +HW::uartCount
#endif
#ifdef USB_SERIAL_LL
        +1
#endif

    ;

    static void maskSerial(SerialIdentifier id) {
        mask |= (1 << id);
    }
    static void unmaskSerial(SerialIdentifier id) {
        mask &= ~(1 << id);
    }
    static bool isMasked(SerialIdentifier id) {
        return (mask & (1 << id)) != 0;
    }

    static void init() {
#ifdef UART_LL
        UartManager<ZP>::init();
#endif
    }

    static Serial* getSerialById(SerialIdentifier id) {
#ifdef UART_LL
        if(id<HW::uartCount){
            return UartManager<ZP>::getUartById(id);
        }
#endif
#ifdef USB_SERIAL_LL
        return &Usb<LL>::usb;
#endif
    }
};
template<class ZP>
uint16_t SerialManager<ZP>::mask = 0;

#endif /* PERIPHERALS_SERIAL_SERIAL_LL_SRC_MAIN_C___SERIAL_LL_LOWLEVEL_SERIALMANAGER_HPP_ */

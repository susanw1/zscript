/*
 * UartManager.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "Uart.hpp"

class Serial;

template<class LL>
class UartManager {
private:
    static Uart<LL> uarts[LL::uartCount];

    static uint16_t mask;

    static void interrupt(uint8_t id);

    UartManager() {
    }

public:
    static void maskSerial(SerialIdentifier id) {
        mask |= (1 << id);
    }
    static void unmaskSerial(SerialIdentifier id) {
        mask &= ~(1 << id);
    }
    static bool isMasked(SerialIdentifier id) {
        return (mask & (1 << id)) != 0;
    }

    static void init();

    static Serial* getUartById(SerialIdentifier id) {
        return uarts + id;
    }
};
template<class ZP>
uint16_t UartManager<ZP>::mask = 0;

#include "specific/UartManagercpp.hpp"

#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_ */

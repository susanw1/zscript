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

class UartManager {
private:

    static Uart uarts[GeneralHalSetup::uartCount];
    UartManager() {
    }
    friend UartInterruptManager;

public:
    static void init();

    static Uart* getUartById(UartIdentifier id) {
        return uarts + id;
    }
};

#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UARTMANAGER_HPP_ */

/*
 * ZcodeUartSubsystem.hpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_UART_ZCODEUARTSUBSYSTEM_HPP_
#define SRC_TEST_C___COMMANDS_UART_ZCODEUARTSUBSYSTEM_HPP_
#include "ZcodeIncludes.hpp"
#include "ZcodeParameters.hpp"
#include "../LowLevel/UartLowLevel/UartManager.hpp"
#include "ZcodeUartInterruptSource.hpp"

class ZcodeUartSubsystem {
public:

    static void uartBufferOverflowCallback(SerialIdentifier id) {
        ZcodeUartInterruptSource::setInterrupt(id);
    }

};

#endif /* SRC_TEST_C___COMMANDS_UART_ZCODEUARTSUBSYSTEM_HPP_ */

/*
 * RCodeUartSubsystem.hpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___COMMANDS_UART_RCODEUARTSUBSYSTEM_HPP_
#define SRC_TEST_C___COMMANDS_UART_RCODEUARTSUBSYSTEM_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "../LowLevel/UartLowLevel/UartManager.hpp"
#include "RCodeUartInterruptSource.hpp"

class RCodeUartSubsystem {
public:

    static void uartBufferOverflowCallback(SerialIdentifier id) {
        RCodeUartInterruptSource::setInterrupt(id);
    }

};

#endif /* SRC_TEST_C___COMMANDS_UART_RCODEUARTSUBSYSTEM_HPP_ */

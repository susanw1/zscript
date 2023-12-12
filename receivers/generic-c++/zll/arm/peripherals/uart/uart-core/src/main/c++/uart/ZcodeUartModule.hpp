/*
 * ZcodeUartModule.hpp
 *
 *  Created on: 16 Dec 2022
 *      Author: alicia
 */

#ifndef PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTMODULE_HPP_
#define PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTMODULE_HPP_

#if defined(UART_LL)
#include <uart-ll/ZcodeUartModule.hpp>
#else
#error No Uart system specified for usage
#endif

#endif /* PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTMODULE_HPP_ */

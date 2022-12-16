/*
 * ZcodeUartChannel.hpp
 *
 *  Created on: 16 Dec 2022
 *      Author: alicia
 */

#ifndef PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTCHANNEL_HPP_
#define PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTCHANNEL_HPP_

#if defined(UART_LL)
#include <uart-ll/ZcodeUartChannel.hpp>
#else
#error No Serial system specified for usage
#endif

#endif /* PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTCHANNEL_HPP_ */

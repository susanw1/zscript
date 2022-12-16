/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTCHANNEL_HPP_
#define PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTCHANNEL_HPP_

#if defined(UART_LL)
#include <uart-ll/ZcodeUartChannel.hpp>
#else
#error No Serial system specified for usage
#endif

#endif /* PERIPHERALS_UART_SRC_MAIN_C___UART_ZCODEUARTCHANNEL_HPP_ */

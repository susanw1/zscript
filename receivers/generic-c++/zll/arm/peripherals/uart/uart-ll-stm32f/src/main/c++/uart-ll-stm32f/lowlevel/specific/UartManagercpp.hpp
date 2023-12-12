/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_

#include "UartInternal.hpp"
#include <utility/PreprocessorArrayInitialiser.hpp>
#include <interrupt-ll/InterruptManager.hpp>
#include <dma-ll/DmaManager.hpp>
#include <uart-ll/lowlevel/UartManager.hpp>

#define UART_TX_Inner(UART,PIN) UART##_TX_##PIN
#define UART_TX(UART,PIN) UART_TX_Inner(UART, PIN)

#define UART_RX_Inner(UART,PIN) UART##_RX_##PIN
#define UART_RX(UART,PIN) UART_RX_Inner(UART, PIN)

#ifdef USE_UART1
#if !UART_TX(_UART1, UART1_TX)
#error  Not defined as a valid UART 1 TX pin: UART1_TX
#endif
#if !UART_RX(_UART1, UART1_RX)
#error  Not defined as a valid UART 1 RX pin: UART1_RX
#endif
#endif

#ifdef USE_UART2
#if !UART_TX(_UART2, UART2_TX)
#error  Not defined as a valid UART 2 TX pin: UART2_TX
#endif
#if !UART_RX(_UART2, UART2_RX)
#error  Not defined as a valid UART 2 RX pin: UART2_RX
#endif
#endif

#ifdef USE_UART3
#if !UART_TX(_UART3, UART3_TX)
#error  Not defined as a valid UART 3 TX pin: UART3_TX
#endif
#if !UART_RX(_UART3, UART3_RX)
#error  Not defined as a valid UART 3 RX pin: UART3_RX
#endif
#endif

#ifdef USE_UART4
#if !UART_TX(_UART4, UART4_TX)
#error  Not defined as a valid UART 4 TX pin: UART4_TX
#endif
#if !UART_RX(_UART4, UART4_RX)
#error  Not defined as a valid UART 4 RX pin: UART4_RX
#endif
#endif

#ifdef USE_UART5
#if !UART_TX(_UART5, UART5_TX)
#error  Not defined as a valid UART 5 TX pin: UART5_TX
#endif
#if !UART_RX(_UART5, UART5_RX)
#error  Not defined as a valid UART 5 RX pin: UART5_RX
#endif
#endif

#ifdef USE_UART6
#if !UART_TX(_UART6, UART6_TX)
#error  Not defined as a valid UART 6 TX pin: UART6_TX
#endif
#if !UART_RX(_UART6, UART6_RX)
#error  Not defined as a valid UART 6 RX pin: UART6_RX
#endif
#endif

#define UART_INITIALISE(x) Uart<LL>(\
        UartInternal<LL>(UART_RX(UART##x, UART##x##_RX), UART_TX(UART##x, UART##x##_TX), UART##x##_REGISTER_LOC, x),\
        DmaManager<LL>::getDmaById(UART##x##_TX_DMA),\
        UART##x##_TX_DMAMUX\
        )

template<class LL>
Uart<LL> UartManager<LL>::uarts[] = {
        ARRAY_INITIALISE_1(UART_INITIALISE, UART_COUNT)
};

template<class LL>
void UartManager<LL>::interrupt(uint8_t id) {
UartManager<LL>::uarts[id].interrupt();
}

template<class LL>
void UartManager<LL>::init() {
InterruptManager::setInterrupt(&UartManager::interrupt, UartInt);
for (int i = 0; i < HW::uartCount; ++i) {
    InterruptManager::enableInterrupt(UartInt, i, 8);
}
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32F0_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_ */

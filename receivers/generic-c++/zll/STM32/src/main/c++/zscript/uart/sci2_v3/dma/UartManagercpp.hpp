/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_

#include "UartInternal.hpp"
#include <arm-no-os/system/interrupt/InterruptManager.hpp>


template<class LL>
Uart<LL> UartManager<LL>::uarts[] = {

#ifdef USE_UART_1
    Uart<LL>()
#endif
#ifdef USE_UART_2
    , Uart<LL>()
#endif
#ifdef USE_UART_3
    , Uart<LL>()
#endif
#ifdef USE_UART_4
    , Uart<LL>()
#endif
#ifdef USE_UART_5
    , Uart<LL>()
#endif
#ifdef USE_UART_6
    , Uart<LL>()
#endif
        };

template<class LL>
void UartManager<LL>::interrupt(uint8_t id) {
    UartManager < LL > ::uarts[id].interrupt();
}

template<class LL>
UartInternal<LL> getUartInternal(SerialIdentifier id) {
    if (id == 0) {
#ifdef USE_UART_1
        return UartInternal<LL>(UART_RX(UART_1, UART_1_RX), UART_TX(UART_1, UART_1_TX), (UartRegisters*) 0x40013800, id);
#else
        return UartInternal<LL>();
#endif
    } else if (id == 1) {
#ifdef USE_UART_2
        return UartInternal<LL>(UART_RX(UART_2, UART_2_RX), UART_TX(UART_2, UART_2_TX), (UartRegisters*) 0x40004400, id);
#else
        return UartInternal<LL>();
#endif
    } else if (id == 2) {
#ifdef USE_UART_3
        return UartInternal<LL>(UART_RX(UART_3, UART_3_RX), UART_TX(UART_3, UART_3_TX), (UartRegisters*) 0x40004800, id);
#else
        return UartInternal<LL>();
#endif
    } else if (id == 3) {
#ifdef USE_UART_4
        return UartInternal<LL>(UART_RX(UART_4, UART_4_RX), UART_TX(UART_4, UART_4_TX), (UartRegisters*) 0x40004C00, id);
#else
        return UartInternal<LL>();
#endif
    } else if (id == 4) {
#ifdef USE_UART_5
        return UartInternal<LL>(UART_RX(UART_5, UART_5_RX), UART_TX(UART_5, UART_5_TX), (UartRegisters*) 0x40005000, id);
#else
        return UartInternal<LL>();
#endif
    } else {
#ifdef USE_UART_6
        return UartInternal<LL>(UART_RX(UART_6, UART_6_RX), UART_TX(UART_6, UART_6_TX), (UartRegisters*) 0x40008000, id);
#else
        return UartInternal<LL>();
#endif
    }
}
DmaMuxRequest getUartMuxTxRequest(SerialIdentifier id) {
    if (id == 0) {
        return DMAMUX_USART1_TX;
    } else if (id == 1) {
        return DMAMUX_USART2_TX;
    } else if (id == 2) {
        return DMAMUX_USART3_TX;
    } else if (id == 3) {
        return DMAMUX_UART4_TX;
    } else if (id == 4) {
        return DMAMUX_UART5_TX;
    } else {
        return DMAMUX_LPUART1_TX;
    }
}

template<class LL>
void UartManager<LL>::init() {
    InterruptManager::setInterrupt(&UartManager::interrupt, UartInt);
    for (int i = 0; i < HW::uartCount; ++i) {
        uarts[i].setUart(getUartInternal < LL > (i), DmaManager < LL > ::getDmaById(LL::uart1TxDma + i), getUartMuxTxRequest(i));
        InterruptManager::enableInterrupt(UartInt, i, 8);
    }
#ifdef USE_USB_SERIAL
    Usb<LL>::usb.setId(HW::uartCount);
#endif
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_SERIAL_MODULE_LOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_ */

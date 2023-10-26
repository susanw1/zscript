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

#define UART_TX_Inner(UART,PIN) UART##_TX_##PIN
#define UART_TX(UART,PIN) UART_TX_Inner(UART, PIN)

#define UART_RX_Inner(UART,PIN) UART##_RX_##PIN
#define UART_RX(UART,PIN) UART_RX_Inner(UART, PIN)

#if defined(STM32G431xx)
#include <arm-no-os/stm32g431/serial-module/lowlevel/specific/UartPins.hpp>
#elif defined(STM32G441xx)
#include <arm-no-os/stm32g441/serial-module/lowlevel/specific/UartPins.hpp>
#elif defined(STM32G471xx)
#include <arm-no-os/stm32g471/serial-module/lowlevel/specific/UartPins.hpp>
#elif defined(STM32G473xx)
#include <arm-no-os/stm32g473/serial-module/lowlevel/specific/UartPins.hpp>
#elif defined(STM32G483xx)
#include <arm-no-os/stm32g483/serial-module/lowlevel/specific/UartPins.hpp>
#elif defined(STM32G474xx)
#include <arm-no-os/stm32g474/serial-module/lowlevel/specific/UartPins.hpp>
#elif defined(STM32G484xx)
#include <arm-no-os/stm32g484/serial-module/lowlevel/specific/UartPins.hpp>
#elif defined(STM32GBK1CB)
#include <arm-no-os/stm32gbk1cb/serial-module/lowlevel/specific/UartPins.hpp>
#else
#error "Please select the target STM32G4xx device used in your application"
#endif

#ifdef USE_UART_1
#if !UART_TX(_UART_1, UART_1_TX)
#error  Not defined as a valid UART 1 TX pin: UART_1_TX
#endif
#if !UART_RX(_UART_1, UART_1_RX)
#error  Not defined as a valid UART 1 RX pin: UART_1_RX
#endif
#endif

#ifdef USE_UART_2
#if !UART_TX(_UART_2, UART_2_TX)
#error  Not defined as a valid UART 2 TX pin: UART_2_TX
#endif
#if !UART_RX(_UART_2, UART_2_RX)
#error  Not defined as a valid UART 2 RX pin: UART_2_RX
#endif
#endif

#ifdef USE_UART_3
#if !UART_TX(_UART_3, UART_3_TX)
#error  Not defined as a valid UART 3 TX pin: UART_3_TX
#endif
#if !UART_RX(_UART_3, UART_3_RX)
#error  Not defined as a valid UART 3 RX pin: UART_3_RX
#endif
#endif

#ifdef USE_UART_4
#if !UART_TX(_UART_4, UART_4_TX)
#error  Not defined as a valid UART 4 TX pin: UART_4_TX
#endif
#if !UART_RX(_UART_4, UART_4_RX)
#error  Not defined as a valid UART 4 RX pin: UART_4_RX
#endif
#endif

#ifdef USE_UART_5
#if !UART_TX(_UART_5, UART_5_TX)
#error  Not defined as a valid UART 5 TX pin: UART_5_TX
#endif
#if !UART_RX(_UART_5, UART_5_RX)
#error  Not defined as a valid UART 5 RX pin: UART_5_RX
#endif
#endif

#ifdef USE_UART_6
#if !UART_TX(_UART_6, UART_6_TX)
#error  Not defined as a valid UART 6 TX pin: UART_6_TX
#endif
#if !UART_RX(_UART_6, UART_6_RX)
#error  Not defined as a valid UART 6 RX pin: UART_6_RX
#endif
#endif

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

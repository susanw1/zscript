/*
 * UartManagercpp.hpp
 *
 *  Created on: 8 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_

#include "../UartManager.hpp"
#include "UartInternal.hpp"
#include <InterruptLowLevel/InterruptManager.hpp>

#define UART_TX_Inner(UART,PIN) UART##_TX_##PIN
#define UART_TX(UART,PIN) UART_TX_Inner(UART, PIN)

#define UART_RX_Inner(UART,PIN) UART##_RX_##PIN
#define UART_RX(UART,PIN) UART_RX_Inner(UART, PIN)

#define UART_1_TX_PA_9_ PA_9,GPIO_AF7
#define UART_1_RX_PA_10_ PA_10,GPIO_AF7
#define UART_1_TX_PB_6_ PB_6,GPIO_AF7
#define UART_1_RX_PB_7_ PB_7,GPIO_AF7
#define UART_1_TX_PC_4_ PC_4,GPIO_AF7
#define UART_1_RX_PC_5_ PC_5,GPIO_AF7
#define UART_1_TX_PE_0_ PE_0,GPIO_AF7
#define UART_1_RX_PE_1_ PE_1,GPIO_AF7
#define UART_1_TX_PG_9_ PG_9,GPIO_AF7

#define UART_2_TX_PA_2_ PA_2,GPIO_AF7
#define UART_2_RX_PA_3_ PA_3,GPIO_AF7
#define UART_2_TX_PA_14_ PA_14,GPIO_AF7
#define UART_2_RX_PA_15_ PA_15,GPIO_AF7
#define UART_2_TX_PB_3_ PB_3,GPIO_AF7
#define UART_2_RX_PB_4_ PB_4,GPIO_AF7
#define UART_2_TX_PD_5_ PD_5,GPIO_AF7
#define UART_2_RX_PD_6_ PD_6,GPIO_AF7

#define UART_3_RX_PB_8_ PB_8,GPIO_AF7
#define UART_3_TX_PB_9_ PB_9,GPIO_AF7
#define UART_3_TX_PB_10_ PB_10,GPIO_AF7
#define UART_3_RX_PB_11_ PB_11,GPIO_AF7
#define UART_3_TX_PC_10_ PC_10,GPIO_AF7
#define UART_3_RX_PC_11_ PC_11,GPIO_AF7
#define UART_3_TX_PD_8_ PD_8,GPIO_AF7
#define UART_3_RX_PD_9_ PD_9,GPIO_AF7

#define UART_4_TX_PC_10_ PC_10,GPIO_AF5
#define UART_4_RX_PC_11_ PC_11,GPIO_AF5

#define UART_5_TX_PC_12_ PC_12,GPIO_AF5
#define UART_5_RX_PD_2_ PD_2,GPIO_AF5

#define UART_6_TX_PA_2_ PA_2,GPIO_AF12
#define UART_6_RX_PA_3_ PA_3,GPIO_AF12
#define UART_6_RX_PB_10_ PB_10,GPIO_AF8
#define UART_6_TX_PB_11_ PB_11,GPIO_AF8
#define UART_6_RX_PC_0_ PC_0,GPIO_AF8
#define UART_6_TX_PC_1_ PC_1,GPIO_AF8
#define UART_6_TX_PG_7_ PG_7,GPIO_AF8
#define UART_6_RX_PG_8_ PG_8,GPIO_AF8

#define _UART_1_TX_PA_9_ 1
#define _UART_1_RX_PA_10_ 1
#define _UART_1_TX_PB_6_ 1
#define _UART_1_RX_PB_7_ 1
#define _UART_1_TX_PC_4_ 1
#define _UART_1_RX_PC_5_ 1
#define _UART_1_TX_PE_0_ 1
#define _UART_1_RX_PE_1_ 1
#define _UART_1_TX_PG_9_ 1

#define _UART_2_TX_PA_2_ 1
#define _UART_2_RX_PA_3_ 1
#define _UART_2_TX_PA_14_ 1
#define _UART_2_RX_PA_15_ 1
#define _UART_2_TX_PB_3_ 1
#define _UART_2_RX_PB_4_ 1
#define _UART_2_TX_PD_5_ 1
#define _UART_2_RX_PD_6_ 1

#define _UART_3_RX_PB_8_ 1
#define _UART_3_TX_PB_9_ 1
#define _UART_3_TX_PB_10_ 1
#define _UART_3_RX_PB_11_ 1
#define _UART_3_TX_PC_10_ 1
#define _UART_3_RX_PC_11_ 1
#define _UART_3_TX_PD_8_ 1
#define _UART_3_RX_PD_9_ 1

#define _UART_4_TX_PC_10_ 1
#define _UART_4_RX_PC_11_ 1

#define _UART_5_TX_PC_12_ 1
#define _UART_5_RX_PD_2_ 1

#define _UART_6_TX_PA_2_ 1
#define _UART_6_RX_PA_3_ 1
#define _UART_6_RX_PB_10_ 1
#define _UART_6_TX_PB_11_ 1
#define _UART_6_RX_PC_0_ 1
#define _UART_6_TX_PC_1_ 1
#define _UART_6_TX_PG_7_ 1
#define _UART_6_RX_PG_8_ 1

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

#ifdef USE_USB_SERIAL
template<class LL>
Usb UartManager<LL>::usb(LL::UsbSerialId);
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
    UartManager<LL>::uarts[id].interrupt();
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
    for (int i = 0; i < GeneralHalSetup::uartCount; ++i) {
        uarts[i].setUart(getUartInternal<LL>(i), DmaManager<LL>::getDmaById(LL::uart1TxDma + i), getUartMuxTxRequest(i));
        InterruptManager::enableInterrupt(UartInt, i, 8);
    }
}

#endif /* SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTMANAGERCPP_HPP_ */

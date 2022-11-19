/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_CPP_TEST_MINIMAL_GENERALLLSETUP_HPP_
#define SRC_TEST_CPP_TEST_MINIMAL_GENERALLLSETUP_HPP_

#include <arm-no-os/llIncludes.hpp>

#define I2C_1_SDA PB_9_
#define I2C_1_SCL PA_15_
//#define USE_I2C_1

#define I2C_2_SDA PA_8_
#define I2C_2_SCL PC_4_
//#define USE_I2C_2

#define I2C_3_SDA PC_9_
#define I2C_3_SCL PC_8_
//#define USE_I2C_3

#define I2C_4_SDA PC_7_
#define I2C_4_SCL PC_6_
//#define USE_I2C_4

#define UART_1_TX PA_9_
#define UART_1_RX PA_10_
#define USE_UART_1

//#define UART_2_TX PA_2_
//#define UART_2_RX PA_3_
//#define USE_UART_2
//
//#define UART_3_TX PB_10_
//#define UART_3_RX PB_11_
//#define USE_UART_3
//
//#define UART_4_TX PC_10_
//#define UART_4_RX PC_11_
//#define USE_UART_4
//
//#define UART_5_TX PC_12_
//#define UART_5_RX PD_2_
//#define USE_UART_5
//
//#define UART_6_TX PC_1_
//#define UART_6_RX PC_0_
//#define USE_UART_6

//#define USE_USB_SERIAL

#define I2C_1_ALERT PC_9
#define I2C_2_ALERT PC_4
#define I2C_3_ALERT PC_4
#define I2C_4_ALERT PC_4

//#define I2C_DMA

#define SPI_1_SCK PA_5_
#define SPI_1_MISO PA_6_
#define SPI_1_MOSI PA_7_
//#define USE_SPI_1

#define SPI_1_UDP_SS PA_4

class LowLevelConfiguration {
public:
    typedef HardwareDefinition HW;
    static const uint16_t persistentGuidLocation = 16;
    static const uint16_t persistentMacAddressLocation = 40;
    static const uint16_t mainPersistentMemoryLocation = 48;

    static const uint16_t UartBufferRxSize = 1024;
    static const uint16_t UartBufferTxSize = 256;  // want rx buffer much larger, as it has to store any data which hasn't yet been read.

    static const DmaIdentifier uart1TxDma = 6;
    static const DmaIdentifier uart2TxDma = 7;
    static const DmaIdentifier uart3TxDma = 8;
    static const DmaIdentifier uart4TxDma = 9;
    static const DmaIdentifier uart5TxDma = 10;
    static const DmaIdentifier uart6TxDma = 11;
    static const uint8_t uartEscapingChar = 0xFE;
};

#endif /* SRC_TEST_CPP_TEST_MINIMAL_GENERALLLSETUP_HPP_ */

/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_TEST_CPP_MAIN_GENERALLLSETUP_HPP_
#define SRC_TEST_CPP_MAIN_GENERALLLSETUP_HPP_

#include <llIncludes.hpp>

#define DMA_LL_STM32G
#define CLOCK_LL_STM32G
#define WIZNET_UDP_LL

#define I2C_LL
#define PINS_LL
#define SERIAL_LL
#define USBC_PD_LL
#define USB_SERIAL_LL
#define UDP_WIZNET_LL

#define I2C_1_SDA PB9_
#define I2C_1_SCL PA15_
#define USE_I2C_1

#define I2C_2_SDA PA8_
#define I2C_2_SCL PC4_
//#define USE_I2C_2

#define I2C_3_SDA PC9_
#define I2C_3_SCL PC8_
//#define USE_I2C_3

#define I2C_4_SDA PC_7_
#define I2C_4_SCL PC_6_
//#define USE_I2C_4

#define UART_1_TX PA9_
#define UART_1_RX PA10_
#define USE_UART_1

#define UART_2_TX PA2_
#define UART_2_RX PA3_
#define USE_UART_2

#define UART_3_TX PB10_
#define UART_3_RX PB11_
#define USE_UART_3

#define UART_4_TX PC10_
#define UART_4_RX PC11_
#define USE_UART_4

#define UART_5_TX PC12_
#define UART_5_RX PD2_
#define USE_UART_5

#define UART_6_TX PC1_
#define UART_6_RX PC0_
#define USE_UART_6

#define USE_USB_SERIAL

#define I2C_1_ALERT PC9_
#define I2C_2_ALERT PC4_
#define I2C_3_ALERT PC4_
#define I2C_4_ALERT PC4_

#define I2C_DMA

#define SPI_1_SCK PA5_
#define SPI_1_MISO PA6_
#define SPI_1_MOSI PA7_
#define USE_SPI_1

#define SPI_1_UDP_SS PA4_

class LowLevelConfiguration {
public:
    static const uint16_t persistentGuidLocation = 16;
    static const uint16_t persistentMacAddressLocation = 40;
    static const uint16_t mainPersistentMemoryLocation = 48;

    static const int ucpdRxBufferSize = 1024;
    static const int ucpdTxBufferSize = 256;

    static const DmaIdentifier ucpdRxDma = 4;
    static const DmaIdentifier ucpdTxDma = 5;

    static const uint16_t ucpdVID = 0;
    static const uint16_t ucpdPID = 0;
    static const uint32_t ucpdXID = 0;

    static const uint8_t ucpdFirmwareRevisionNum = 0;
    static const uint8_t ucpdHardwareRevisionNum = 0;

    static const uint8_t ucpdMinPower = 0;
    static const uint8_t ucpdOperationalPower = 0;
    static const uint8_t ucpdMaxPower = 0;

    static const char *ucpdManufacturerInfo;

    static const uint16_t UsbBufferRxSize = 1024;
    static const uint16_t UsbBufferTxSize = 1024;

    static const uint16_t UartBufferRxSize = 1024;
    static const uint16_t UartBufferTxSize = 256;  // want rx buffer much larger, as it has to store any data which hasn't yet been read.

    static const DmaIdentifier i2c1Dma = 0;
    static const DmaIdentifier i2c2Dma = 1;
    static const DmaIdentifier i2c3Dma = 2;
    static const DmaIdentifier i2c4Dma = 3;

    static const DmaIdentifier uart1TxDma = 6;
    static const DmaIdentifier uart2TxDma = 7;
    static const DmaIdentifier uart3TxDma = 8;
    static const DmaIdentifier uart4TxDma = 9;
    static const DmaIdentifier uart5TxDma = 10;
    static const DmaIdentifier uart6TxDma = 11;
    static const uint8_t uartEscapingChar = 0xFE;
};

#endif /* SRC_TEST_CPP_MAIN_GENERALLLSETUP_HPP_ */

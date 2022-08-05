/*
 * GeneralHalSetup.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_

#include <arm-no-os/llIncludes.hpp>

#define I2C_1_SDA PB_9_
#define I2C_1_SCL PA_15_
#define USE_I2C_1

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

#define UART_2_TX PA_2_
#define UART_2_RX PA_3_
#define USE_UART_2

#define UART_3_TX PB_10_
#define UART_3_RX PB_11_
#define USE_UART_3

#define UART_4_TX PC_10_
#define UART_4_RX PC_11_
#define USE_UART_4

#define UART_5_TX PC_12_
#define UART_5_RX PD_2_
#define USE_UART_5

#define UART_6_TX PC_1_
#define UART_6_RX PC_0_
#define USE_UART_6

#define USE_USB_SERIAL

#define I2C_1_ALERT PC_9
#define I2C_2_ALERT PC_4
#define I2C_3_ALERT PC_4
#define I2C_4_ALERT PC_4

#define I2C_DMA

class LowLevelConfiguration {
public:
    typedef HardwareDefinition HW;
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
#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_ */

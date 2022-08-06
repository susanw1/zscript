/*
 * GeneralHalSetup.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_

#include <arm-no-os/llIncludes.hpp>

#define I2C_1_SDA PB_7_
#define I2C_1_SCL PB_6_
//#define USE_I2C_1

#define I2C_2_SDA PA_8_
#define I2C_2_SCL PC_4_
//#define USE_I2C_2

#define UART_1_TX PA_9_
#define UART_1_RX PA_10_
#define USE_UART_1

#define UART_2_TX PA_2_
#define UART_2_RX PA_3_
//#define USE_UART_2

#define I2C_1_ALERT PA_8
#define I2C_2_ALERT PB_4

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

    static const DmaIdentifier uart1TxDma = 1;
    static const DmaIdentifier uart2TxDma = 3;

    static const uint8_t uartEscapingChar = 0xFE;
};
#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_ */

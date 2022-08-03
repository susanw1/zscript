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
#define USE_I2C_1

#define I2C_2_SDA PA_8_
#define I2C_2_SCL PC_4_
//#define USE_I2C_2

#define UART_1_TX PA_9_
#define UART_1_RX PA_10_
#define USE_UART_1

#define UART_2_TX PA_2_
#define UART_2_RX PA_3_
#define USE_UART_2

#define USE_USB_SERIAL

#define I2C_1_ALERT PA_8
#define I2C_2_ALERT PB_4

class HardwareDefinition {
public:
    static const uint32_t ucpdTxSOP = 0b10001110001100011000; // 3*Sync-1, Sync-2 = SOP

    static const uint8_t pinCount = 55;

    static const uint8_t atoDCount = 5;

    static const I2cIdentifier i2cCount = 2;
    static const SerialIdentifier uartCount = 2;
    static const SerialIdentifier serialCount = 2;

    static const DmaIdentifier dmaCount = 5;

    static const bool inDualBankFlash = true;
    static const uint32_t pageSize = 0x400;
};

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

    static const uint16_t UartBufferRxSize = 1024;
    static const uint16_t UartBufferTxSize = 256;  // want rx buffer much larger, as it has to store any data which hasn't yet been read.

    static const DmaIdentifier uart1TxDma = 1;
    static const DmaIdentifier uart2TxDma = 3;

    static const uint8_t uartEscapingChar = 0xFE;
};
#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_GENERALHALSETUP_HPP_ */

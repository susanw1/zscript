/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file    USB_Device/CDC_Standalone/Core/Src/main.c
 * @author  MCD Application Team
 * @brief   USB device CDC demo main file
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2019 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under Ultimate Liberty license
 * SLA0044, the "License"; You may not use this file except in compliance with
 * the License. You may obtain a copy of the License at:
 *                             www.st.com/SLA0044
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/

#include <stm32g4xx.h>
#include <stm32g484xx.h>
#include "ZcodeParameters.hpp"

#include <ArduinoSpiLayer/src/Ethernet.h>

#include <Zcode.hpp>
#include <commands/ZcodeActivateCommand.hpp>
#include <commands/ZcodeSetDebugChannelCommand.hpp>
#include <commands/ZcodeEchoCommand.hpp>
#include <commands/ZcodeExecutionStateCommand.hpp>
#include <commands/ZcodeExecutionCommand.hpp>
#include <commands/ZcodeExecutionStoreCommand.hpp>
#include <commands/ZcodeNotificationHostCommand.hpp>
#include <commands/ZcodeCapabilitiesCommand.hpp>
#include <executionspace/ZcodeExecutionSpaceChannel.hpp>
#include <ZcodeIdentifyCommand.hpp>

#include <EthernetUdpCommandChannel.hpp>

#include "../commands/persistence/ZcodeFetchGuidCommand.hpp"
#include "../commands/persistence/ZcodePersistentFetchCommand.hpp"
#include "../commands/persistence/ZcodePersistentStoreCommand.hpp"
#include "../commands/persistence/ZcodeStoreGuidCommand.hpp"
#include "../commands/persistence/ZcodeStoreMacAddressCommand.hpp"
#include "../commands/persistence/ZcodeFlashPersistence.hpp"

#include <I2C/ZcodeI2cSubsystem.hpp>
#include <I2C/ZcodeI2cSetupCommand.hpp>
#include <I2C/ZcodeI2cSendCommand.hpp>
#include <I2C/ZcodeI2cReceiveCommand.hpp>

#include <Pins/ZcodePinSystem.hpp>
#include <Pins/ZcodePinSetupCommand.hpp>
#include <Pins/ZcodePinSetCommand.hpp>
#include <Pins/ZcodePinGetCommand.hpp>
#include <Pins/ZcodePinInterruptSource.hpp>

#include <Uart/ZcodeUartSubsystem.hpp>
#include <Uart/ZcodeUartSetupCommand.hpp>
#include <Uart/ZcodeUartSendCommand.hpp>
#include <Uart/ZcodeUartReadCommand.hpp>
#include <Uart/ZcodeUartAvailableCommand.hpp>
#include <Uart/ZcodeUartSkipCommand.hpp>

#include "../LowLevel/AToDLowLevel/AtoD.hpp"
#include "../LowLevel/AToDLowLevel/AtoDManager.hpp"

#include "../LowLevel/GpioLowLevel/GpioManager.hpp"
#include "../LowLevel/GpioLowLevel/Gpio.hpp"

#include "../LowLevel/ClocksLowLevel/SystemMilliClock.hpp"
#include "../LowLevel/ClocksLowLevel/ClockManager.hpp"
#include "../LowLevel/ClocksLowLevel/Clock.hpp"

#include "../LowLevel/I2cLowLevel/I2cManager.hpp"
#include "../LowLevel/I2cLowLevel/I2c.hpp"

#include "../LowLevel/UartLowLevel/UartManager.hpp"
#include "../LowLevel/UartLowLevel/Uart.hpp"

#include "stm32g4xx.h"
#include "stm32g484xx.h"

void doNothing(I2c *i2c, I2cTerminationStatus status) {
    i2c->unlock();
}
int main(void) {
    ClockManager::getClock(VCO)->set(300000, HSI);
    ClockManager::getClock(PLL_R)->set(150000, VCO);
    ClockManager::getClock(SysClock)->set(150000, PLL_R);
    ClockManager::getClock(HCLK)->set(150000, SysClock);
    ClockManager::getClock(PCLK_1)->set(64000, HCLK);
    ClockManager::getClock(PCLK_2)->set(64000, HCLK);
    ZcodePinInterruptSource::init();
    ZcodePinInterruptSource source;
    DmaManager::init();
    GpioManager::init();
    I2cManager::init();
    UartManager::init();
    SystemMilliClock::init();
    SystemMilliClock::blockDelayMillis(1000);
    ZcodeFlashPersistence persist;
    ZcodeBusInterruptSource<ZcodeParameters> *sources[] = { &source };
    Zcode<ZcodeParameters> z(sources, 1);
    uint8_t *mac;
    uint8_t macHardCoded[6] = { 0xde, 0xad, 0xbe, 0xef, 0xfe, 0xaa };
    if (persist.hasMac()) {
        z.getDebug() << "Has Mac" << endl;
        mac = persist.getMac();
    } else {
        z.getDebug() << "Has No Mac" << endl;
        mac = macHardCoded;
    }
    while (!Ethernet.begin(mac, 5000, 5000)) {

    }
    AtoDManager::init();
    EthernetUdpCommandChannel channel(4889, &z);
    ZcodeExecutionSpaceChannel<ZcodeParameters> execCh(&z, z.getSpace());
    ZcodeCommandChannel<ZcodeParameters> *chptr[2] = { &channel, &execCh };
    ZcodeExecutionSpaceChannel<ZcodeParameters> **execPtr = (ZcodeExecutionSpaceChannel<ZcodeParameters>**) chptr + 1;
    z.setChannels(chptr, 2);
    z.getSpace()->setChannels(execPtr, 1);
    ZcodeEchoCommand<ZcodeParameters> cmd0;
    ZcodeActivateCommand<ZcodeParameters> cmd1;
    ZcodeSetDebugChannelCommand<ZcodeParameters> cmd2(&z);
    ZcodeCapabilitiesCommand<ZcodeParameters> cmd3(&z);
    ZcodeExecutionStateCommand<ZcodeParameters> cmd4(z.getSpace());
    ZcodeExecutionCommand<ZcodeParameters> cmd5(z.getSpace());
    ZcodeExecutionStoreCommand<ZcodeParameters> cmd6(z.getSpace());
    ZcodeNotificationHostCommand<ZcodeParameters> cmd7(&z);
    ZcodeIdentifyCommand cmd8 = ZcodeIdentifyCommand();

    ZcodeFetchGuidCommand cmd9(&persist);
    ZcodePersistentFetchCommand cmd10(&persist);
    ZcodePersistentStoreCommand cmd11(&persist);
    ZcodeStoreGuidCommand cmd12(&persist);
    ZcodeStoreMacAddressCommand cmd13(&persist);

    ZcodeI2cSubsystem::init();
    ZcodeI2cSetupCommand cmd14;
    ZcodeI2cSendCommand cmd15;
    ZcodeI2cReceiveCommand cmd16;

    ZcodeUartSetupCommand cmd24;
    ZcodeUartSendCommand cmd25;
    ZcodeUartReadCommand cmd26;
    ZcodeUartAvailableCommand cmd27;
    ZcodeUartSkipCommand cmd28;

    ZcodePinSetupCommand cmd17;
    ZcodePinSetCommand cmd18;
    ZcodePinGetCommand cmd19;

    z.getCommandFinder()->registerCommand(&cmd8);
    z.getCommandFinder()->registerCommand(&cmd0);
    z.getCommandFinder()->registerCommand(&cmd3);
    z.getCommandFinder()->registerCommand(&cmd1);
    z.getCommandFinder()->registerCommand(&cmd7);
    z.getCommandFinder()->registerCommand(&cmd2);
    z.getCommandFinder()->registerCommand(&cmd4);
    z.getCommandFinder()->registerCommand(&cmd5);
    z.getCommandFinder()->registerCommand(&cmd6);

    z.getCommandFinder()->registerCommand(&cmd9);
    z.getCommandFinder()->registerCommand(&cmd10);
    z.getCommandFinder()->registerCommand(&cmd11);
    z.getCommandFinder()->registerCommand(&cmd12);
    z.getCommandFinder()->registerCommand(&cmd13);

    z.getCommandFinder()->registerCommand(&cmd14);
    z.getCommandFinder()->registerCommand(&cmd15);
    z.getCommandFinder()->registerCommand(&cmd16);

    z.getCommandFinder()->registerCommand(&cmd24);
    z.getCommandFinder()->registerCommand(&cmd25);
    z.getCommandFinder()->registerCommand(&cmd26);
    z.getCommandFinder()->registerCommand(&cmd27);
    z.getCommandFinder()->registerCommand(&cmd28);

    z.getCommandFinder()->registerCommand(&cmd17);
    z.getCommandFinder()->registerCommand(&cmd18);
    z.getCommandFinder()->registerCommand(&cmd19);

    I2c *i2c1 = I2cManager::getI2cById(0);
    i2c1->init();
    GpioPin *c4 = GpioManager::getPin(PC_4);
    c4->init();
    c4->setOutputMode(PushPull);
    c4->setPullMode(NoPull);
    c4->setOutputSpeed(MediumSpeed);
    c4->setMode(Output);
    c4->set();
    if (!i2c1->isLocked()) {
        i2c1->lock();
        uint8_t data[2] = { 0x0A, 0x80 };
        i2c1->asyncTransmit(0x20, false, data, 2, &doNothing);
    }
    while (i2c1->isLocked())
        ;
    if (!i2c1->isLocked()) {
        i2c1->lock();
        uint8_t data[2] = { 0, 0 };
        i2c1->asyncTransmit(0x20, false, data, 2, &doNothing);
    }
    while (i2c1->isLocked())
        ;
    if (!i2c1->isLocked()) {
        i2c1->lock();
        uint8_t data[2] = { 0x16, 0xFF };
        i2c1->asyncTransmit(0x20, false, data, 2, &doNothing);
    }
    while (true) {
//        AtoDManager::performAtoD(PA_9);
//        AtoDManager::getAtoD(4)->performReading(2);
//        uint16_t result = AtoDManager::getAtoD(4)->performReading(2);
//        SystemMilliClock::blockDelayMillis (time);
//        if (read != 0xff) {
//            time = 500;
//        } else {
//            time = 1000;
//        }
//        c4->reset();
//        if (!i2c1->isLocked()) {
//            i2c1->lock();
//            uint8_t data[2] = { 0x19 };
//            i2c1->asyncTransmitReceive(0x20, data, 1, &read, 1, &doNothing);
//        }
//        if (!i2c1->isLocked()) {
//            i2c1->lock();
//            i2c1->asyncReceive(0x20, &read, 1, &doNothing);
//        }
//        SystemMilliClock::blockDelayMillis(time);
//        c4->set();
//        if (!i2c1->isLocked()) {
//            on = !on;
//            i2c1->lock();
//            uint8_t data[2] = { 0x0A, 0xFF };
//            if (on) {
//                data[1] = 0;
//            }
//            i2c1->asyncTransmit(0x20, data, 2, &doNothing);
//        }
        z.progressZcode();
        Ethernet.maintain();
//        uip.tick();
//        uip.dhcpClient.checkLease();
    }
}
//    wait_us(5000000);
//    uint8_t mac[] = { 0x1E, 0xAD, 0xBE, 0xEF, 0xFE, 0xED };
//    UipEthernet uip(mac, PA_7, PA_6, PA_5, PA_4);
//    while (uip.connect(5))
//        ;
//    I2cManager::init();
//    Zcode z(NULL, 0);
//    UipUdpCommandChannel channel(&z, &uip, 4889);
//    ZcodeExecutionSpaceChannel execCh = ZcodeExecutionSpaceChannel(&z, z.getSpace());
//    ZcodeCommandChannel *chptr[2] = { &channel, &execCh };
//    ZcodeExecutionSpaceChannel **execPtr = (ZcodeExecutionSpaceChannel**) chptr + 1;
//    z.setChannels(chptr, 2);
//    z.getSpace()->setChannels(execPtr, 1);
//    ZcodeEchoCommand cmd0 = ZcodeEchoCommand();
//    ZcodeActivateCommand cmd1 = ZcodeActivateCommand();
//    ZcodeSetDebugChannelCommand cmd2 = ZcodeSetDebugChannelCommand(&z);
//    ZcodeCapabilitiesCommand cmd3 = ZcodeCapabilitiesCommand(&z);
//    ZcodeExecutionStateCommand cmd4 = ZcodeExecutionStateCommand(z.getSpace());
//    ZcodeExecutionCommand cmd5 = ZcodeExecutionCommand(z.getSpace());
//    ZcodeExecutionStoreCommand cmd6 = ZcodeExecutionStoreCommand(z.getSpace());
//    ZcodeNotificationHostCommand cmd7 = ZcodeNotificationHostCommand(&z);
//    ZcodeIdentifyCommand cmd8 = ZcodeIdentifyCommand();

//    ZcodeMbedFlashPersistence persist = ZcodeMbedFlashPersistence();
//    ZcodeFetchGuidCommand cmd9 = ZcodeFetchGuidCommand(&persist);
//    ZcodePersistentFetchCommand cmd10 = ZcodePersistentFetchCommand(&persist);
//    ZcodePersistentStoreCommand cmd11 = ZcodePersistentStoreCommand(&persist);
//    ZcodeStoreGuidCommand cmd12 = ZcodeStoreGuidCommand(&persist);
//    ZcodeStoreMacAddressCommand cmd13 = ZcodeStoreMacAddressCommand(&persist);

//    ZcodeI2cSubsystem::init();
//    ZcodeI2cSetupCommand cmd14 = ZcodeI2cSetupCommand();
//    ZcodeI2cSendCommand cmd15 = ZcodeI2cSendCommand();
//    z.getCommandFinder()->registerCommand(&cmd8);
//    z.getCommandFinder()->registerCommand(&cmd0);
//    z.getCommandFinder()->registerCommand(&cmd3);
//    z.getCommandFinder()->registerCommand(&cmd1);
//    z.getCommandFinder()->registerCommand(&cmd7);
//    z.getCommandFinder()->registerCommand(&cmd2);
//    z.getCommandFinder()->registerCommand(&cmd4);
//    z.getCommandFinder()->registerCommand(&cmd5);
//    z.getCommandFinder()->registerCommand(&cmd6);

//    z.getCommandFinder()->registerCommand(&cmd9);
//    z.getCommandFinder()->registerCommand(&cmd10);
//    z.getCommandFinder()->registerCommand(&cmd11);
//    z.getCommandFinder()->registerCommand(&cmd12);
//    z.getCommandFinder()->registerCommand(&cmd13);

//    z.getCommandFinder()->registerCommand(&cmd14);
//    z.getCommandFinder()->registerCommand(&cmd15);
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

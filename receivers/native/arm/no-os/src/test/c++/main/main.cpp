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

#include <arm-no-os/system/core/stm32g4xx.h>
#include <arm-no-os/system/core/stm32g484xx.h>
#include <arm-no-os/system/core/core_cm4.h>

#include "ZcodeParameters.hpp"
#include <arm-no-os/GeneralLLSetup.hpp>

#include <arm-no-os/i2c-module/addressing/ZcodeI2cAddressingSystem.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialAddressingSystem.hpp>
#include <zcode/modules/core/ZcodeDebugAddressingSystem.hpp>
#include <zcode/addressing/addressrouters/ZcodeModuleAddressRouter.hpp>

#include <arm-no-os/arm-core-module/commands/ZcodeReadGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeWriteGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeResetCommand.hpp>

#include <arm-no-os/serial-module/ZcodeSerialModule.hpp>
#include <arm-no-os/usbc-pd-module/ZcodeUsbcPDModule.hpp>
#include <arm-no-os/i2c-module/ZcodeI2cModule.hpp>

#include <zcode/modules/script/ZcodeScriptModule.hpp>
#include <zcode/modules/outer-core/ZcodeOuterCoreModule.hpp>
#include <zcode/modules/core/ZcodeCoreModule.hpp>

#include <arm-no-os/udp-module/channels/EthernetUdpChannel.hpp>
#include <arm-no-os/serial-module/channels/SerialChannel.hpp>

#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>
#include <arm-no-os/pins-module/AToDLowLevel/AtoDManager.hpp>

#include <arm-no-os/system/clock/SystemMilliClock.hpp>
#include <arm-no-os/system/clock/ClockManager.hpp>

#include <arm-no-os/i2c-module/lowlevel/I2cManager.hpp>

#include <arm-no-os/usbc-pd-module/lowlevel/Ucpd.hpp>

#include <arm-no-os/serial-module/lowlevel/UartManager.hpp>

#include <arm-no-os/udp-module/lowlevel/src/Ethernet.h>

#include <arm-no-os/i2c-module/addressing/ZcodeI2cBusInterruptSource.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialBusInterruptSource.hpp>

#include <zcode/Zcode.hpp>

const char *GeneralHalSetup::ucpdManufacturerInfo = "Zcode/Alpha Board";

int main(void) {
    ClockManager<GeneralHalSetup>::getClock(VCO)->set(240000, HSI);
    ClockManager<GeneralHalSetup>::getClock(PLL_R)->set(120000, VCO);
    ClockManager<GeneralHalSetup>::getClock(SysClock)->set(120000, PLL_R);
    ClockManager<GeneralHalSetup>::getClock(HCLK)->set(120000, SysClock);
    ClockManager<GeneralHalSetup>::getClock(PCLK_1)->set(60000, HCLK);
    ClockManager<GeneralHalSetup>::getClock(PCLK_2)->set(60000, HCLK);
    DmaManager<GeneralHalSetup>::init();
    GpioManager<GeneralHalSetup>::init();
    I2cManager<GeneralHalSetup>::init();
    SystemMilliClock<GeneralHalSetup>::init();
    UartManager<GeneralHalSetup>::init();
    SystemMilliClock<GeneralHalSetup>::blockDelayMillis(1000);
    uint32_t notSoPermanentStore = 0;
    Ucpd<GeneralHalSetup>::init(&notSoPermanentStore, 150, 5 * 20, 5 * 20);

    ZcodeI2cBusInterruptSource<ZcodeParameters> i2cSource;
    ZcodeSerialBusInterruptSource<ZcodeParameters> serialSource;

    Zcode<ZcodeParameters> *z = &Zcode<ZcodeParameters>::zcode;

    AtoDManager<GeneralHalSetup>::init();
    SystemMilliClock<GeneralHalSetup>::blockDelayMillis(1000);
    Usb<GeneralHalSetup>::usb.init(NULL, 0, false);

    EthernetUdpChannel<ZcodeParameters> channel(4889);
    SerialChannel<ZcodeParameters> serial(&Usb<GeneralHalSetup>::usb);
    ZcodeCommandChannel<ZcodeParameters> *chptr[] = { &channel, &serial };
    z->setChannels(chptr, 2);

    ZcodeBusInterruptSource<ZcodeParameters> *sources[] = { &i2cSource, &serialSource };
    z->setInterruptSources(sources, 2);

    I2c<GeneralHalSetup> *i2c1 = I2cManager<GeneralHalSetup>::getI2cById(0);
    GpioPin<GeneralHalSetup> *c4 = GpioManager<GeneralHalSetup>::getPin(PC_4);
    c4->init();
    c4->setOutputMode(PushPull);
    c4->setPullMode(NoPull);
    c4->setOutputSpeed(MediumSpeed);
    c4->setMode(Output);
    c4->set();
    if (i2c1->lock()) {
        uint8_t data1[2] = { 0x0A, 0x80 };
        i2c1->transmit(0x20, data1, 2);

        uint8_t data2[2] = { 0, 0 };
        i2c1->transmit(0x20, data2, 2);

        uint8_t data3[2] = { 0x16, 0xFF };
        i2c1->transmit(0x20, data3, 2);
        i2c1->unlock();
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
        z->progressZcode();
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

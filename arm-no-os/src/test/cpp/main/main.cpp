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

#include "stm32g4xx.h"
#include "stm32g484xx.h"

//#include "RCode.hpp"
//#include "commands/RCodeActivateCommand.hpp"
//#include "commands/RCodeSetDebugChannelCommand.hpp"
//#include "commands/RCodeEchoCommand.hpp"
//#include "commands/RCodeExecutionStateCommand.hpp"
//#include "commands/RCodeExecutionCommand.hpp"
//#include "commands/RCodeExecutionStoreCommand.hpp"
//#include "commands/RCodeNotificationHostCommand.hpp"
//#include "commands/RCodeCapabilitiesCommand.hpp"
//#include "executionspace/RCodeExecutionSpaceChannel.hpp"
//#include "../commands/RCodeIdentifyCommand.hpp"

//#include "../commands/persistence/RCodeFetchGiudCommand.hpp"
//#include "../commands/persistence/RCodePersistentFetchCommand.hpp"
//#include "../commands/persistence/RCodePersistentStoreCommand.hpp"
//#include "../commands/persistence/RCodeStoreGiudCommand.hpp"
//#include "../commands/persistence/RCodeStoreMacAddressCommand.hpp"
//#include "../commands/persistence/RCodeMbedFlashPersistence.hpp"
//#include "../commands/I2C/RCodeI2cSubsystem.hpp"
//#include "../commands/I2C/RCodeI2cSetupCommand.hpp"
//#include "../commands/I2C/RCodeI2cSendCommand.hpp"

#include "../LowLevel/GpioLowLevel/GpioManager.hpp"
#include "../LowLevel/GpioLowLevel/Gpio.hpp"

#include "../LowLevel/ClocksLowLevel/ClockManager.hpp"
#include "../LowLevel/ClocksLowLevel/Clock.hpp"

#include "../LowLevel/I2cLowLevel/I2cManager.hpp"
#include "../LowLevel/I2cLowLevel/I2c.hpp"

#include "stm32g4xx.h"
#include "stm32g484xx.h"

void doNothing(I2c *i2c, I2cTerminationStatus status) {
    i2c->unlock();
}
int main(void) {
    for (volatile uint32_t i = 0; i < 0x10000; ++i)
        ;
    ClockManager::getClock(VCO)->set(300000, HSI);
    ClockManager::getClock(PLL_R)->set(150000, VCO);
    ClockManager::getClock(SysClock)->set(150000, PLL_R);
    ClockManager::getClock(HCLK)->set(150000, SysClock);
    ClockManager::getClock(PCLK_1)->set(64000, HCLK);
    GpioManager::init();
    I2cManager::init();
    I2c *i2c1 = I2cManager::getI2cById(0);
    i2c1->init();
    GpioPin *c4 = GpioManager::getPin(PC_4);
    c4->init();
    c4->setOutputMode(PushPull);
    c4->setPullMode(NoPull);
    c4->setOutputSpeed(MediumSpeed);
    c4->setMode(Output);
    c4->set();
    GpioPin *c8 = GpioManager::getPin(PC_8);
    c8->init();
    c8->setPullMode(PullDown);
    c8->setMode(Input);
    uint8_t addr = 0;
    uint32_t time = 0x100000;
    while (true) {
        if (c8->read()) {
            time = 0x80000;
        } else {
            time = 0x100000;
        }
        for (volatile uint32_t i = 0; i < time; ++i)
            ;
        c4->reset();
        if (c8->read()) {
            time = 0x80000;
        } else {
            time = 0x100000;
        }
        for (volatile uint32_t i = 0; i < time; ++i)
            ;
        c4->set();
        if (!i2c1->isLocked()) {
            i2c1->lock();
            uint8_t data[4] = { 0xff, 0, 0xff, 0 };
            i2c1->asyncTransmit(0x20, data, 4, &doNothing);
            if (addr > 127) {
                addr = 0;
            }
        }
//        r.progressRCode();
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
//    RCode r(NULL, 0);
//    UipUdpCommandChannel channel(&r, &uip, 4889);
//    RCodeExecutionSpaceChannel execCh = RCodeExecutionSpaceChannel(&r, r.getSpace());
//    RCodeCommandChannel *chptr[2] = { &channel, &execCh };
//    RCodeExecutionSpaceChannel **execPtr = (RCodeExecutionSpaceChannel**) chptr + 1;
//    r.setChannels(chptr, 2);
//    r.getSpace()->setChannels(execPtr, 1);
//    RCodeEchoCommand cmd0 = RCodeEchoCommand();
//    RCodeActivateCommand cmd1 = RCodeActivateCommand();
//    RCodeSetDebugChannelCommand cmd2 = RCodeSetDebugChannelCommand(&r);
//    RCodeCapabilitiesCommand cmd3 = RCodeCapabilitiesCommand(&r);
//    RCodeExecutionStateCommand cmd4 = RCodeExecutionStateCommand(r.getSpace());
//    RCodeExecutionCommand cmd5 = RCodeExecutionCommand(r.getSpace());
//    RCodeExecutionStoreCommand cmd6 = RCodeExecutionStoreCommand(r.getSpace());
//    RCodeNotificationHostCommand cmd7 = RCodeNotificationHostCommand(&r);
//    RCodeIdentifyCommand cmd8 = RCodeIdentifyCommand();

//    RCodeMbedFlashPersistence persist = RCodeMbedFlashPersistence();
//    RCodeFetchGiudCommand cmd9 = RCodeFetchGiudCommand(&persist);
//    RCodePersistentFetchCommand cmd10 = RCodePersistentFetchCommand(&persist);
//    RCodePersistentStoreCommand cmd11 = RCodePersistentStoreCommand(&persist);
//    RCodeStoreGiudCommand cmd12 = RCodeStoreGiudCommand(&persist);
//    RCodeStoreMacAddressCommand cmd13 = RCodeStoreMacAddressCommand(&persist);

//    RCodeI2cSubsystem::init();
//    RCodeI2cSetupCommand cmd14 = RCodeI2cSetupCommand();
//    RCodeI2cSendCommand cmd15 = RCodeI2cSendCommand();
//    r.getCommandFinder()->registerCommand(&cmd8);
//    r.getCommandFinder()->registerCommand(&cmd0);
//    r.getCommandFinder()->registerCommand(&cmd3);
//    r.getCommandFinder()->registerCommand(&cmd1);
//    r.getCommandFinder()->registerCommand(&cmd7);
//    r.getCommandFinder()->registerCommand(&cmd2);
//    r.getCommandFinder()->registerCommand(&cmd4);
//    r.getCommandFinder()->registerCommand(&cmd5);
//    r.getCommandFinder()->registerCommand(&cmd6);

//    r.getCommandFinder()->registerCommand(&cmd9);
//    r.getCommandFinder()->registerCommand(&cmd10);
//    r.getCommandFinder()->registerCommand(&cmd11);
//    r.getCommandFinder()->registerCommand(&cmd12);
//    r.getCommandFinder()->registerCommand(&cmd13);

//    r.getCommandFinder()->registerCommand(&cmd14);
//    r.getCommandFinder()->registerCommand(&cmd15);
/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

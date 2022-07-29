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

#include <arm-no-os/system/core/stm32f0xx.h>
#include <arm-no-os/system/core/stm32f030x8.h>
#include <arm-no-os/system/core/core_cm0.h>

#include "ZcodeParameters.hpp"
#include <arm-no-os/GeneralLLSetup.hpp>

#include <arm-no-os/i2c-module/addressing/ZcodeI2cAddressingSystem.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialAddressingSystem.hpp>
#include <zcode/modules/core/ZcodeDebugAddressingSystem.hpp>

#include <zcode/addressing/addressrouters/ZcodeModuleAddressRouter.hpp>

//#include <arm-no-os/arm-core-module/commands/ZcodeReadGuidCommand.hpp>
//#include <arm-no-os/arm-core-module/commands/ZcodeWriteGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeResetCommand.hpp>

#include <arm-no-os/pins-module/ZcodePinModule.hpp>
#include <arm-no-os/serial-module/ZcodeSerialModule.hpp>
#include <arm-no-os/i2c-module/ZcodeI2cModule.hpp>

//#include <zcode/modules/script/ZcodeScriptModule.hpp>
#include <zcode/modules/outer-core/ZcodeOuterCoreModule.hpp>
#include <zcode/modules/core/ZcodeCoreModule.hpp>

#include <arm-no-os/udp-module/channels/EthernetUdpChannel.hpp>
#include <arm-no-os/serial-module/channels/SerialChannel.hpp>

#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>
//#include <arm-no-os/pins-module/AToDLowLevel/AtoDManager.hpp>

#include <arm-no-os/system/clock/SystemMilliClock.hpp>
#include <arm-no-os/system/clock/ClockManager.hpp>

#include <arm-no-os/i2c-module/lowlevel/I2cManager.hpp>

#include <arm-no-os/serial-module/lowlevel/UartManager.hpp>

#include <arm-no-os/i2c-module/addressing/ZcodeI2cBusInterruptSource.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialBusInterruptSource.hpp>

#include <zcode/Zcode.hpp>

const char *GeneralHalSetup::ucpdManufacturerInfo = "Zcode/Alpha Board";

int main(void) {
    ClockManager<GeneralHalSetup>::getClock(HSI)->set(8000, NONE);
    ClockManager<GeneralHalSetup>::getClock(PLL)->set(40000, HSI);
    ClockManager<GeneralHalSetup>::getClock(SysClock)->set(40000, PLL);
    ClockManager<GeneralHalSetup>::getClock(HCLK)->set(40000, SysClock);
    ClockManager<GeneralHalSetup>::getClock(PCLK)->set(20000, HCLK);

    SystemMilliClock<GeneralHalSetup>::init();
    for (volatile uint32_t i = 0; i < 0x100; ++i)
        ;
    SystemMilliClock<GeneralHalSetup>::blockDelayMillis(20);
    DmaManager<GeneralHalSetup>::init();
    GpioManager<GeneralHalSetup>::init();
    UartManager<GeneralHalSetup>::init();
    GpioManager<GeneralHalSetup>::getPin(PC_9).init();
    GpioManager<GeneralHalSetup>::getPin(PC_9).setMode(Output);
    I2cManager<GeneralHalSetup>::init();

    ZcodeI2cBusInterruptSource<ZcodeParameters> i2cSource;
    ZcodeSerialBusInterruptSource<ZcodeParameters> serialSource;
    Zcode<ZcodeParameters> *z = &Zcode<ZcodeParameters>::zcode;

    SystemMilliClock<GeneralHalSetup>::blockDelayMillis(1000);

    EthernetUdpChannel<ZcodeParameters> channel(4889);
    SerialChannel<ZcodeParameters> serial(UartManager<GeneralHalSetup>::getUartById(0));
    ZcodeCommandChannel<ZcodeParameters> *chptr[] = { &channel, &serial };
    z->setChannels(chptr, 2);

    ZcodeBusInterruptSource<ZcodeParameters> *sources[] = { &i2cSource, &serialSource };
    z->setInterruptSources(sources, 2);

    GpioManager<GeneralHalSetup>::getPin(PC_9).set();

    while (true) {
        z->progressZcode();
        Ethernet<GeneralHalSetup> .maintain();
    }
}

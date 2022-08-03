#include <arm-no-os/system/core/stm32f0xx.h>
#include <arm-no-os/system/core/stm32f030x8.h>
#include <arm-no-os/system/core/core_cm0.h>

#include "ZcodeParameters.hpp"
#include "GeneralLLSetup.hpp"

#include <arm-no-os/i2c-module/addressing/ZcodeI2cAddressingSystem.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialAddressingSystem.hpp>
#include <zcode/modules/core/ZcodeDebugAddressingSystem.hpp>

#include <zcode/addressing/addressrouters/ZcodeModuleAddressRouter.hpp>

#include <arm-no-os/arm-core-module/commands/ZcodeReadGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeWriteGuidCommand.hpp>
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

const char *LowLevelConfiguration::ucpdManufacturerInfo = "Zcode/Alpha Board";

int main(void) {
    ClockManager<LowLevelConfiguration>::basicSetup();

    SystemMilliClock<LowLevelConfiguration>::init();
    for (volatile uint32_t i = 0; i < 0x100; ++i)
        ;
    SystemMilliClock<LowLevelConfiguration>::blockDelayMillis(20);
    DmaManager<LowLevelConfiguration>::init();
    GpioManager<LowLevelConfiguration>::init();
    UartManager<LowLevelConfiguration>::init();
    GpioManager<LowLevelConfiguration>::getPin(PC_9).init();
    GpioManager<LowLevelConfiguration>::getPin(PC_9).setMode(Output);
    I2cManager<LowLevelConfiguration>::init();

    ZcodeI2cBusInterruptSource<ZcodeParameters> i2cSource;
    ZcodeSerialBusInterruptSource<ZcodeParameters> serialSource;
    Zcode<ZcodeParameters> *z = &Zcode<ZcodeParameters>::zcode;

    SystemMilliClock<LowLevelConfiguration>::blockDelayMillis(1000);

    EthernetUdpChannel<ZcodeParameters> channel(4889);
    SerialChannel<ZcodeParameters> serial(UartManager<LowLevelConfiguration>::getUartById(0));
    ZcodeCommandChannel<ZcodeParameters> *chptr[] = { &channel, &serial };
    z->setChannels(chptr, 2);

    ZcodeBusInterruptSource<ZcodeParameters> *sources[] = { &i2cSource, &serialSource };
    z->setInterruptSources(sources, 2);

    GpioManager<LowLevelConfiguration>::getPin(PC_9).set();

    while (true) {
        z->progressZcode();
        Ethernet<LowLevelConfiguration> .maintain();
    }
}

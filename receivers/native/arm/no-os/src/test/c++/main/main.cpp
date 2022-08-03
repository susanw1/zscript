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

#include <arm-no-os/pins-module/ZcodePinModule.hpp>
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

#include <arm-no-os/i2c-module/addressing/ZcodeI2cBusInterruptSource.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialBusInterruptSource.hpp>

#include <zcode/Zcode.hpp>

const char *GeneralHalSetup::ucpdManufacturerInfo = "Zcode/Alpha Board";

int main(void) {
    ClockManager<GeneralHalSetup>::basicSetup();

    SystemMilliClock<GeneralHalSetup>::init();
    for (volatile uint32_t i = 0; i < 0x100; ++i)
        ;
    SystemMilliClock<GeneralHalSetup>::blockDelayMillis(20);
    DmaManager<GeneralHalSetup>::init();
    GpioManager<GeneralHalSetup>::init();
    I2cManager<GeneralHalSetup>::init();
    uint32_t notSoPermanentStore = 0;
    Ucpd<GeneralHalSetup>::init(&notSoPermanentStore, 150, 5 * 20, 5 * 20);
    UartManager<GeneralHalSetup>::init();

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
        z->progressZcode();
        Ethernet<GeneralHalSetup> .maintain();
    }
}


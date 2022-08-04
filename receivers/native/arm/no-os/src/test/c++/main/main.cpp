#include "ZcodeParameters.hpp"
#include "GeneralLLSetup.hpp"

#include <arm-no-os/i2c-module/addressing/ZcodeI2cAddressingSystem.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialAddressingSystem.hpp>
#include <zcode/modules/core/ZcodeDebugAddressingSystem.hpp>

#include <zcode/addressing/addressrouters/ZcodeModuleAddressRouter.hpp>

#include <arm-no-os/arm-core-module/commands/ZcodeReadGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeWriteGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeResetCommand.hpp>

#include <arm-no-os/system/ZcodeSystemModule.hpp>
#include <arm-no-os/pins-module/ZcodePinModule.hpp>
#include <arm-no-os/serial-module/ZcodeSerialModule.hpp>
#include <arm-no-os/usbc-pd-module/ZcodeUsbcPDModule.hpp>
#include <arm-no-os/i2c-module/ZcodeI2cModule.hpp>

#include <zcode/modules/script/ZcodeScriptModule.hpp>
#include <zcode/modules/outer-core/ZcodeOuterCoreModule.hpp>
#include <zcode/modules/core/ZcodeCoreModule.hpp>

#include <arm-no-os/udp-module/channels/EthernetUdpChannel.hpp>
#include <arm-no-os/serial-module/channels/SerialChannel.hpp>

#include <arm-no-os/i2c-module/addressing/ZcodeI2cBusInterruptSource.hpp>
#include <arm-no-os/serial-module/addressing/ZcodeSerialBusInterruptSource.hpp>

#include <zcode/Zcode.hpp>

const char *LowLevelConfiguration::ucpdManufacturerInfo = "Zcode/Alpha Board";

int main(void) {
    ZcodeSystemModule<ZcodeParameters>::init();

    ZcodePinModule<ZcodeParameters>::init();
    uint32_t notSoPermanentStore = 0;
    ZcodeUsbcPDModule<ZcodeParameters>::init(&notSoPermanentStore, 1500, 5);
    ZcodeI2cModule<ZcodeParameters>::init();
    ZcodeSerialModule<ZcodeParameters>::init();

    ZcodeI2cBusInterruptSource<ZcodeParameters> i2cSource;
    ZcodeSerialBusInterruptSource<ZcodeParameters> serialSource;

    Zcode<ZcodeParameters> *z = &Zcode<ZcodeParameters>::zcode;

    ZcodeSystemModule<ZcodeParameters>::milliClock::blockDelayMillis(200);

    EthernetUdpChannel<ZcodeParameters> channel(4889);
    SerialChannel<ZcodeParameters> serial(ZcodeSerialModule<ZcodeParameters>::getUsb());
    ZcodeCommandChannel<ZcodeParameters> *chptr[] = { &channel, &serial };
    z->setChannels(chptr, 2);

    ZcodeBusInterruptSource<ZcodeParameters> *sources[] = { &i2cSource, &serialSource };
    z->setInterruptSources(sources, 2);

    I2c<LowLevelConfiguration> *i2c1 = ZcodeI2cModule<ZcodeParameters>::getI2c(0);
    if (i2c1->lock()) {
        uint8_t data1[2] = { 0x0A, 0x80 };
        i2c1->transmit(0x20, data1, sizeof(data1));

        uint8_t data2[2] = { 0, 0 };
        i2c1->transmit(0x20, data2, sizeof(data2));

        uint8_t data3[2] = { 0x16, 0xFF };
        i2c1->transmit(0x20, data3, sizeof(data3));
        i2c1->unlock();
    }
    while (true) {
        ZcodeUsbcPDModule<ZcodeParameters>::tick();
        z->progressZcode();
    }
}


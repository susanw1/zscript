#include "ZcodeParameters.hpp"
#include "GeneralLLSetup.hpp"

#include <arm-no-os/arm-core-module/commands/ZcodeReadGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeWriteGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeResetCommand.hpp>

#include <arm-no-os/system/ZcodeSystemModule.hpp>
#include <arm-no-os/pins-module/ZcodePinModule.hpp>
#include <arm-no-os/serial-module/ZcodeSerialModule.hpp>
#include <arm-no-os/i2c-module/ZcodeI2cModule.hpp>
#include <arm-no-os/udp-module/ZcodeUdpModule.hpp>

//#include <zcode/modules/script/ZcodeScriptModule.hpp>
#include <zcode/modules/outer-core/ZcodeOuterCoreModule.hpp>
#include <zcode/modules/core/ZcodeCoreModule.hpp>

#include <zcode/addressing/addressrouters/ZcodeModuleAddressRouter.hpp>
#include <zcode/Zcode.hpp>

const char *LowLevelConfiguration::ucpdManufacturerInfo = "Zcode/Alpha Board";

int main(void) {
    ZcodeSystemModule<ZcodeParameters>::initClocksFast();
    ZcodeSystemModule<ZcodeParameters>::init();

    ZcodePinModule<ZcodeParameters>::init();
    ZcodeI2cModule<ZcodeParameters>::init();
    ZcodeSerialModule<ZcodeParameters>::init();
    ZcodePinModule<ZcodeParameters>::getPin(PC_9).init();
    ZcodePinModule<ZcodeParameters>::getPin(PC_9).setMode(Output);

    ZcodeI2cModule<ZcodeParameters>::busInterruptSource i2cSource;
    ZcodeSerialModule<ZcodeParameters>::busInterruptSource serialSource;
    Zcode<ZcodeParameters> *z = &Zcode<ZcodeParameters>::zcode;

    ZcodeSystemModule<ZcodeParameters>::milliClock::blockDelayMillis(200);

    ZcodeUdpModule<ZcodeParameters>::channel channel(4889);
    ZcodeSerialModule<ZcodeParameters>::channel serial(ZcodeSerialModule<ZcodeParameters>::getUart(0));
    ZcodeCommandChannel<ZcodeParameters> *chptr[] = { &channel, &serial };
    z->setChannels(chptr, 2);

    ZcodeBusInterruptSource<ZcodeParameters> *sources[] = { &i2cSource, &serialSource };
    z->setInterruptSources(sources, 2);

    ZcodePinModule<ZcodeParameters>::getPin(PC_9).set();

    while (true) {
        z->progressZcode();
    }
}

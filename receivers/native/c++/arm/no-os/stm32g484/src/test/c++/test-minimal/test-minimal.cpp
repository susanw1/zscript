/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#define INCLUDE_CPP_IN_FILE_ONLY_USE_ONCE

#include "ZcodeParameters.hpp"
#include "GeneralLLSetup.hpp"

//#include <arm-no-os/arm-core-module/commands/ZcodeReadGuidCommand.hpp>
//#include <arm-no-os/arm-core-module/commands/ZcodeWriteGuidCommand.hpp>
#include <arm-no-os/arm-core-module/commands/ZcodeResetCommand.hpp>

#include <arm-no-os/system/ZcodeSystemModule.hpp>
#include <arm-no-os/pins-module/ZcodePinModule.hpp>
#include <arm-no-os/serial-module/ZcodeSerialModule.hpp>

//#include <zcode/modules/outer-core/ZcodeOuterCoreModule.hpp>
#include <zcode/modules/core/ZcodeCoreModule.hpp>

#include <zcode/Zcode.hpp>

int main(void) {
    ZcodeSystemModule < ZcodeParameters > ::initClocksFast();
    ZcodeSystemModule < ZcodeParameters > ::init();

    ZcodePinModule<ZcodeParameters>::init();
    ZcodeSerialModule<ZcodeParameters>::init();

    Zcode<ZcodeParameters> *z = &Zcode<ZcodeParameters>::zcode;

    ZcodeSystemModule < ZcodeParameters > ::milliClock::blockDelayMillis(200);

    ZcodeSerialModule<ZcodeParameters>::channel serial(ZcodeSerialModule<ZcodeParameters>::getUart(0));
    ZcodeCommandChannel<ZcodeParameters> *chptr[] = { &serial };
    z->setChannels(chptr, 1);

    while (true) {
        z->progressZcode();
    }
}


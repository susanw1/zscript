/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_RUNNING_ZCODERUNNINGCOMMANDSLOTFINDERINC_HPP_
#define SRC_MAIN_CPP_ZCODE_RUNNING_ZCODERUNNINGCOMMANDSLOTFINDERINC_HPP_

#include "ZcodeRunningCommandSlot.hpp"
#include "ZcodeCommandFinder.hpp"

template<class ZP>
void ZcodeRunningCommandSlot<ZP>::performRunAction() {
    if (!status.isStarted) {
        if (!status.hasWrittenTerminator) {
            writeTerminator(commandSlot->starter);
        }
        dataPointer = NULL;
        storedData = 0;
        status.isStarted = true;
    }
    status.needsAction = false;
    status.hasWrittenTerminator = false;
    ZcodeCommandFinder<ZP>::runCommand(this);
    if (status.isComplete) {
        if (!status.hasWrittenTerminator) {
            if (commandSlot->terminator == EOL_SYMBOL) {
                writeTerminator(EOL_SYMBOL);
            } else {
                status.hasWrittenTerminator = false;
            }
        }
        finish();
    }
}

#endif /* SRC_MAIN_CPP_ZCODE_RUNNING_ZCODERUNNINGCOMMANDSLOTFINDERINC_HPP_ */

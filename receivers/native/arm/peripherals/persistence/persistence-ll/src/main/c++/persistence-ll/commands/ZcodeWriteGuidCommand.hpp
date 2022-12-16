/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_ARM_CORE_MODULE_COMMANDS_ZCODEWRITEGUIDCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_ARM_CORE_MODULE_COMMANDS_ZCODEWRITEGUIDCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/arm-core-module/persistence/ZcodeFlashPersistence.hpp>

#define COMMAND_EXISTS_0014 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeWriteGuidCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;

public:
    static constexpr uint8_t CODE = 0x04;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        if (slot.getBigField()->getLength() != 16) {
            slot.fail(BAD_PARAM, "GUIDs must be 16 bytes long");
        } else {
            ZcodeFlashPersistence<LL>::persist.writeGuid(slot.getBigField()->getData());
            out->writeStatus(OK);
        }
    }

};

#define ZCODE_CORE_WRITE_GUID ZcodeWriteGuidCommand<ZP>::execute

#endif /* SRC_MAIN_CPP_ARM_NO_OS_ARM_CORE_MODULE_COMMANDS_ZCODEWRITEGUIDCOMMAND_HPP_ */

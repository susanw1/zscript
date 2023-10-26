/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_ARM_CORE_MODULE_COMMANDS_ZCODEREADGUIDCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_ARM_CORE_MODULE_COMMANDS_ZCODEREADGUIDCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/arm-core-module/persistence/ZcodeFlashPersistence.hpp>

#define COMMAND_EXISTS_0004 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeReadGuidCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;

public:
    static constexpr uint8_t CODE = 0x04;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        if (!ZcodeFlashPersistence<LL>::persist.hasGuid()) {
            slot.fail(CMD_FAIL, "No GUID set");
            return;
        }
        out->writeBigHexField(ZcodeFlashPersistence<LL>::persist.getGuid(), 16);
        out->writeStatus(OK);
    }

};
#define ZCODE_CORE_FETCH_GUID ZcodeReadGuidCommand<ZP>::execute

#endif /* SRC_MAIN_CPP_ARM_NO_OS_ARM_CORE_MODULE_COMMANDS_ZCODEREADGUIDCOMMAND_HPP_ */

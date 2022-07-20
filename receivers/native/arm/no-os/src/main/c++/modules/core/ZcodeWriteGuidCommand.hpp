/*
 * ZcodeWriteGuidCommand.hpp
 *
 *  Created on: 18 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___MODULES_CORE_ZCODEWRITEGUIDCOMMAND_HPP_
#define SRC_MAIN_C___MODULES_CORE_ZCODEWRITEGUIDCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>
#include <LowLevel/PersistenceLowLevel/FlashPage.hpp>
#include "ZcodeFlashPersistence.hpp"

#define COMMAND_VALUE_0014 MODULE_CAPABILITIES_UTIL

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
#endif /* SRC_MAIN_C___MODULES_CORE_ZCODEWRITEGUIDCOMMAND_HPP_ */

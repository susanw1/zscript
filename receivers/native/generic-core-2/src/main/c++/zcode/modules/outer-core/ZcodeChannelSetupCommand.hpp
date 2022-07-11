/*
 * ZcodeChannelSetupCommand.hpp
 *
 *  Created on: 11 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODECHANNELSETUPCOMMAND_HPP_
#define SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODECHANNELSETUPCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeChannelSetupCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x18;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        Zcode<ZP> *zcode = slot.getZcode();
        ZcodeCommandChannel<ZP> *target = slot.getChannel();
        uint16_t targetIndex = 0;
        if (slot.getFields()->get('C', &targetIndex)) {
            if (targetIndex >= zcode->getChannelCount()) {
                slot.fail(BAD_PARAM, ZP::Strings::failBadChannelTarget);
                return;
            } else {
                target = zcode->getChannel((uint8_t) targetIndex);
            }
        }
        if (slot.getFields()->has('N')) {
            zcode->getNotificationManager()->setNotificationChannel(target);
        }
        if (slot.getFields()->has('D')) {
            zcode->getDebug().setDebugChannel(target);
        }
        target->readSetup(slot);
    }

};

#endif /* SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODECHANNELSETUPCOMMAND_HPP_ */

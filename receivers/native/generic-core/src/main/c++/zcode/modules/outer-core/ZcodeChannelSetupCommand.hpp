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

#define COMMAND_EXISTS_0018 EXISTENCE_MARKER_UTIL

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
#ifdef ZCODE_GENERATE_NOTIFICATIONS
        if (slot.getFields()->has('N')) {
            zcode->getNotificationManager()->setNotificationChannel(target);
        }
#endif
#ifdef ZCODE_SUPPORT_DEBUG
        if (slot.getFields()->has('D')) {
            zcode->getDebug().setDebugChannel(target);
        }
#endif
        target->readSetup(slot);
    }

};

#endif /* SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODECHANNELSETUPCOMMAND_HPP_ */

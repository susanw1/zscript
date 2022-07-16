/*
 * ZcodeChannelInfoCommand.hpp
 *
 *  Created on: 11 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODECHANNELINFOCOMMAND_HPP_
#define SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODECHANNELINFOCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_VALUE_0008 MODULE_CAPABILITIES_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeChannelInfoCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x08;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        Zcode<ZP> *zcode = slot.getZcode();
        ZcodeCommandChannel<ZP> *channel = slot.getChannel();
        uint8_t curIndex = 0;
        for (uint8_t i = 0; i < zcode->getChannelCount(); ++i) {
            if (channel == zcode->getChannel(i)) {
                curIndex = i;
                break;
            }
        }
        uint16_t targetIndex = curIndex;
        if (slot.getFields()->get('C', &targetIndex)) {
            if (targetIndex >= zcode->getChannelCount()) {
                slot.fail(BAD_PARAM, ZP::Strings::failBadChannelTarget);
                return;
            }
        }
        out->writeField8('C', zcode->getChannelCount());
        out->writeField8('U', curIndex);
        zcode->getChannel((uint8_t) targetIndex)->giveInfo(slot);
    }

};

#endif /* SRC_MAIN_C___ZCODE_MODULES_CORE_ZCODECHANNELINFOCOMMAND_HPP_ */
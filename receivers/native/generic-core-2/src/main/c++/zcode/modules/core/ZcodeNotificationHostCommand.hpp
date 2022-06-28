/*
 * ZcodeNotificationHostCommand.hpp
 *
 *  Created on: 30 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODENOTIFICATIONHOSTCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODENOTIFICATIONHOSTCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeDebugOutput;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeNotificationHostCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x08;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();

        slot.getZcode()->getNotificationManager()->setNotificationChannel(slot.getChannel());
        out->writeStatus(OK);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODENOTIFICATIONHOSTCOMMAND_HPP_ */

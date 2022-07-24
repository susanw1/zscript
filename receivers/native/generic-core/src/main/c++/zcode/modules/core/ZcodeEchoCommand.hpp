/*
 * ZcodeEchoCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEECHOCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEECHOCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"

#define COMMAND_EXISTS_0001 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class ZcodeEchoCommand: public ZcodeCommand<ZP> {
private:
    typedef typename ZP::Strings::string_t string_t;
    static const uint8_t CODE = 0x01;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t statusResult;
        if (!slot.getFields()->get(Zchars::STATUS_RESP_PARAM, &statusResult)) {
            out->writeStatus(OK);
        } else {
            slot.fail((ZcodeResponseStatus) statusResult, (string_t) NULL);
        }
        slot.getFields()->copyTo(out);
        slot.getBigField()->copyTo(out);
    }
};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEECHOCOMMAND_HPP_ */
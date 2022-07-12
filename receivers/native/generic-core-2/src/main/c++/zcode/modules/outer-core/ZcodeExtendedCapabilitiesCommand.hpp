/*
 * ZcodeExtendedCapabilitiesCommand.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_HPP_
#define SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_HPP_

#include "../../ZcodeIncludes.hpp"
#include "../ZcodeCommand.hpp"
#include "../ZcodeModule.hpp"

#define COMMAND_VALUE_0010 MODULE_CAPABILITIES_UTIL

template<class ZP>
class ZcodeOutStream;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeExtendedCapabilitiesCommand: public ZcodeCommand<ZP> {
private:
    static const uint8_t CODE = 0x10;
    typedef typename ZP::Strings::string_t string_t;

public:

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        uint16_t target;
        if (slot.getFields()->get('M', &target)) {
            if (target >= 16) {
                slot.fail(BAD_PARAM, (string_t) ZP::Strings::badModuleAreaSpecifier);
                return;
            }
            uint16_t result = 0;
            switch (target) {
            case 0:
                result = COMMAND_SWITCH_BYTE(0);
                break;
            case 1:
                result = COMMAND_SWITCH_BYTE(1);
                break;
            case 2:
                result = COMMAND_SWITCH_BYTE(2);
                break;
            case 3:
                result = COMMAND_SWITCH_BYTE(3);
                break;
            case 4:
                result = COMMAND_SWITCH_BYTE(4);
                break;
            case 5:
                result = COMMAND_SWITCH_BYTE(5);
                break;
            case 6:
                result = COMMAND_SWITCH_BYTE(6);
                break;
            case 7:
                result = COMMAND_SWITCH_BYTE(7);
                break;
            case 8:
                result = COMMAND_SWITCH_BYTE(8);
                break;
            case 9:
                result = COMMAND_SWITCH_BYTE(9);
                break;
            case 10:
                result = COMMAND_SWITCH_BYTE(A);
                break;
            case 11:
                result = COMMAND_SWITCH_BYTE(B);
                break;
            case 12:
                result = COMMAND_SWITCH_BYTE(C);
                break;
            case 13:
                result = COMMAND_SWITCH_BYTE(D);
                break;
            case 14:
                result = COMMAND_SWITCH_BYTE(E);
                break;
            case 15:
                result = COMMAND_SWITCH_BYTE(F);
                break;
            default:
                break;
            }
            out->writeField16('L', result);
        }
        out->writeField16('C', MODULE_CAPABILITIES(001));
        out->writeField16('M', BROAD_MODULE_EXISTENCE);

        out->writeStatus(OK);
    }

};

#endif /* SRC_TEST_CPP_ZCODE_COMMANDS_ZCODEEXTENDEDCAPABILITIESCOMMAND_HPP_ */

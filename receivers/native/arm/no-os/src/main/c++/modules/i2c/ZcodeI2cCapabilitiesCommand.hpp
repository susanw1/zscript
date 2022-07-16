#ifndef SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CCAPABILITIESCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>

template<class ZP>
class ZcodeI2cCapabilitiesCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x00;

    static constexpr char CMD_RESP_COMMANDS_C = 'C';
    static constexpr char CMD_RESP_NOTIFICATION_SUPPORTED_N = 'N';
    static constexpr char CMD_RESP_PORT_COUNT_P = 'P';
    static constexpr char CMD_RESP_MAX_FREQUENCIES_SUPPORTED_F = 'F';

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        uint8_t commandBitset = 0x1f; // FIXME - generate from list of enabled commands
        out->writeField8(CMD_RESP_COMMANDS_C, commandBitset);

        out->writeField8(CMD_RESP_NOTIFICATION_SUPPORTED_N, 0);

        out->writeField8(CMD_RESP_PORT_COUNT_P, ZP::LL::i2cCount);
        out->writeField8(CMD_RESP_MAX_FREQUENCIES_SUPPORTED_F, 3);
    }

};

#endif /* SRC_MAIN_CPP_ZCODE_COMMANDS_ZCODEI2CCAPABILITIESCOMMAND_HPP_ */

/*
 * ZcodeUsbcPDCapabilitiesCommand.hpp
 *
 *  Created on: 16 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDCAPABILITIESCOMMAND_HPP_
#define SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDCAPABILITIESCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>

#define COMMAND_EXISTS_0110 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeUsbcPDCapabilitiesCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x00;

    static constexpr char CMD_RESP_COMMANDS_C = 'C';
    static constexpr char CMD_RESP_NOTIFICATION_SUPPORTED_N = 'N';
    static constexpr char CMD_RESP_PORT_COUNT_P = 'P';

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeStatus(OK);
        out->writeField8(CMD_RESP_COMMANDS_C, MODULE_CAPABILITIES(011));
        out->writeField8(CMD_RESP_NOTIFICATION_SUPPORTED_N, 0);
        out->writeField8(CMD_RESP_PORT_COUNT_P, 1);
    }

};
#endif /* SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDCAPABILITIESCOMMAND_HPP_ */

/*
 * ZcodeSerialCapabilitiesCommand.hpp
 *
 *  Created on: 20 Jul 2022
 *      Author: robert
 */

#ifndef NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALCAPABILITIESCOMMAND_HPP_
#define NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALCAPABILITIESCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>

#define COMMAND_EXISTS_0070 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeSerialCapabilitiesCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;
    typedef typename LL::HW HW;

public:
    static constexpr uint8_t CODE = 0x00;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField8('C', MODULE_CAPABILITIES(007));

        out->writeField8('N', 0);
        out->writeField8('A', 0);

        out->writeField8('U', 0x2); // [....., parity, single/double stop, auto board rate]
        out->writeField8('P', HW::serialCount);
        out->writeField32('F', UartManager<LL>::getUartById(0)->getMaxBaud());
        out->writeField16('B', LL::UartBufferTxSize);
        out->writeField16('R', LL::UartBufferRxSize);
        out->writeStatus(OK);
    }

};

#endif /* NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALCAPABILITIESCOMMAND_HPP_ */

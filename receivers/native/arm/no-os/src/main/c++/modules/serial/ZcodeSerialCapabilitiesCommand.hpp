/*
 * ZcodeSerialCapabilitiesCommand.hpp
 *
 *  Created on: 20 Jul 2022
 *      Author: robert
 */

#ifndef NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALCAPABILITIESCOMMAND_HPP_
#define NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALCAPABILITIESCOMMAND_HPP_

#include <modules/ZcodeCommand.hpp>

#define COMMAND_VALUE_0070 MODULE_CAPABILITIES_UTIL

template<class ZP>
class ZcodeSerialCapabilitiesCommand: public ZcodeCommand<ZP> {
    typedef typename ZP::LL LL;

public:
    static constexpr uint8_t CODE = 0x00;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        out->writeField8('C', MODULE_CAPABILITIES(007));

        out->writeField8('N', 0);
        out->writeField8('A', 0);

        out->writeField8('U', 0x2); // [....., parity, single/double stop, auto board rate]

        out->writeField8('P', ZP::LL::serialCount);
        out->writeField32('F', ClockManager<LL>::getClock(SysClock)->getFreqKhz() * 1000 / 16);
        out->writeField16('B', LL::UartBufferTxSize);
        out->writeField16('R', LL::UartBufferRxSize);
        out->writeField8('C', MODULE_CAPABILITIES(007));
        out->writeStatus(OK);
    }

};

#endif /* NO_OS_SRC_MAIN_C___MODULES_SERIAL_ZCODESERIALCAPABILITIESCOMMAND_HPP_ */

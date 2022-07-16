/*
 * ZcodeUsbcPDSourceCommand.hpp
 *
 *  Created on: 16 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSOURCECOMMAND_HPP_
#define SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSOURCECOMMAND_HPP_
#include <modules/ZcodeCommand.hpp>
#include <LowLevel/UsbcPD/Ucpd.hpp>

#define COMMAND_VALUE_0113 MODULE_CAPABILITIES_UTIL

template<class ZP>
class ZcodeUsbcPDSourceCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x03;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        const UcpdSourceCapabilitiesMessage<typename ZP::LL> *message = Ucpd<typename ZP::LL>::getSourceCapMessage();
        uint16_t pdoIndex;
        if (Ucpd<typename ZP::LL>::getPowerStatus() == UnresponsiveSource) {
            slot.fail(CMD_FAIL, "Source unresponsive - possibly not PD compatible");
            return;
        }
        out->writeField8('N', Ucpd<typename ZP::LL>::getSourceCapMessage()->getPdoNum());
        if (slot.getFields()->get('N', &pdoIndex)) {
            if (pdoIndex >= message->getPdoNum()) {
                slot.fail(BAD_PARAM, "PDO index too large");
                return;
            }
            const PowerDataObject<typename ZP::LL> *pdo = message->getPdos() + pdoIndex;
            out->writeField16('I', pdo->getMaximumCurrent());
            out->writeField8('V', pdo->getMinimumVoltage() / 20);

            out->writeField16('A', pdo->getMinimumVoltage());
            if (pdo->getMinimumVoltage() != pdo->getMaximumVoltage()) {
                out->writeField16('M', pdo->getMaximumVoltage());
            }
            out->writeStatus(OK);
        } else {
            if (message->isUsbCommunicationsCapable()) {
                out->writeField8('C', 0);
            }
            if (message->hasUnconstrainedPower()) {
                out->writeField8('U', 0);
            }
            if (message->isUsbSuspendRequired()) {
                out->writeField8('O', 0);
            }
            if (message->hasDualRoleData()) {
                out->writeField8('D', 0);
            }
            out->writeStatus(OK);
        }
    }

};

#endif /* SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSOURCECOMMAND_HPP_ */

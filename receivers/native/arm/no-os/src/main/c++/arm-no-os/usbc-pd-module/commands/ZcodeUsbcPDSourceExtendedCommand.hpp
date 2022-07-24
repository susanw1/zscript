/*
 * ZcodeUsbcPDSourceExtendedCommand.hpp
 *
 *  Created on: 16 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSOURCEEXTENDEDCOMMAND_HPP_
#define SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSOURCEEXTENDEDCOMMAND_HPP_
#include <zcode/modules/ZcodeCommand.hpp>
#include <arm-no-os/usbc-pd-module/lowlevel/Ucpd.hpp>

#define COMMAND_EXISTS_0115 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeUsbcPDSourceExtendedCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x05;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        const UcpdParsedExtendedSourceCapabilitiesMessage<typename ZP::LL> *caps = Ucpd<typename ZP::LL>::getExtendedSourceCapMessage();
        const UcpdParsedManufacturerInfoMessage<typename ZP::LL> *manufacturer = Ucpd<typename ZP::LL>::getManufacturerInfoMessage();
        if (caps->isValid()) {
            out->writeField16('M', caps->getVID());
            out->writeField16('P', caps->getPID());
            out->writeField32('X', caps->getXID());

            out->writeField16('V', (((uint16_t) caps->getFwVersion()) << 8) | caps->getHwVersion());

            out->writeField8('R', (caps->getVoltageRegulation().loadStepMagnitude << 4) | caps->getVoltageRegulation().loadStepRate);
            out->writeField8('H', caps->getHoldupTime());
            out->writeField8('C', caps->getCompliance().getField());

            out->writeField8('G', caps->getTouchCurrent().getField());

            if (caps->getSourceInputs().externalSupplyPresent) {
                out->writeField8('I', caps->getSourceInputs().externalSupplyUnconstrained ? 1 : 0);
            }

            out->writeField8('T', caps->getTouchTemp());

            if (caps->getSourceInputs().internalBatteryPresent) {
                out->writeField16('B', ((((uint16_t) caps->getNumberOfHotswappableBatteries()) << 8) | caps->getNumberOfFixedBatteries()));
            }
            out->writeField8('Q', caps->getSourcePdpRating());
        } else if (manufacturer->isValid()) {
            out->writeField16('M', manufacturer->getVID());
            out->writeField16('P', manufacturer->getPID());
        } else {
            slot.fail(CMD_FAIL, "No source information available");
        }
        if (manufacturer->isValid()) {
            out->writeBigStringField(manufacturer->getData(), manufacturer->getDataLength());
        }
    }

};

#endif /* SRC_MAIN_C___MODULES_USBCPD_ZCODEUSBCPDSOURCEEXTENDEDCOMMAND_HPP_ */

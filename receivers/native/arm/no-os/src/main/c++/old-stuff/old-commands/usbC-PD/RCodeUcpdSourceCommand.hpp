/*
 * RCodeUcpdSourceCommand.hpp
 *
 *  Created on: 7 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSOURCECOMMAND_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSOURCECOMMAND_HPP_
#include "RCodeUcpdSystem.hpp"
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeOutStream.hpp"
#include "commands/RCodeCommand.hpp"

class RCodeUcpdSourceCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code[2] = { 0x11, 0x12 };

public:

    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out) {
        if (slot->getFields()->has('N')) {
            uint8_t pdoIndex = slot->getFields()->getByte('N', 0, 0);
            if (pdoIndex < Ucpd::getSourceCapMessage()->getPdoNum()) {
                out->writeStatus(OK);
                const PowerDataObject *pdo = Ucpd::getSourceCapMessage()->getPdos() + pdoIndex;
                out->writeField('I', (uint16_t) pdo->getMaximumCurrent());
                out->writeField('V', (uint8_t) (pdo->getMinimumVoltage() / 20));

                out->writeField('A', (uint16_t) pdo->getMinimumVoltage());
                if (pdo->getMinimumVoltage() != pdo->getMaximumVoltage()) {
                    out->writeField('M', (uint16_t) pdo->getMaximumVoltage());
                }
            } else {
                out->writeStatus(BAD_PARAM);
                out->writeBigStringField("PDO index too large");
            }
        } else {
            if (Ucpd::getPowerStatus() == UnresponsiveSource) {
                out->writeStatus(CMD_FAIL);
                out->writeBigStringField("Source unresponsive - possibly not PD compatible");
            } else {
                out->writeStatus(OK);
                if (Ucpd::getExtendedSourceCapMessage()->isValid()) {
                    const UcpdParsedExtendedSourceCapabilitiesMessage *extended = Ucpd::getExtendedSourceCapMessage();
                    out->writeField('I', (uint64_t) ((((uint64_t) extended->getXID()) << 32) | (((uint64_t) extended->getVID()) << 16) | extended->getPID()));
                    out->writeField('V', (uint16_t) ((extended->getFwVersion() << 8) | extended->getHwVersion()));

                    out->writeField('R', (uint8_t) (extended->getVoltageRegulation().loadStepMagnitude << 2 | extended->getVoltageRegulation().loadStepRate));

                    uint16_t peak1 = 0;
                    peak1 |= extended->getPeekCurrent1().overloadCurrent;
                    peak1 |= extended->getPeekCurrent1().overloadPeriod << 5;
                    peak1 |= extended->getPeekCurrent1().dutyCycle << 11;
                    peak1 |= extended->getPeekCurrent1().vBusVoltageDroop ? 0x8000 : 0;

                    uint16_t peak2 = 0;
                    peak2 |= extended->getPeekCurrent2().overloadCurrent;
                    peak2 |= extended->getPeekCurrent2().overloadPeriod << 5;
                    peak2 |= extended->getPeekCurrent2().dutyCycle << 11;
                    peak2 |= extended->getPeekCurrent2().vBusVoltageDroop ? 0x8000 : 0;

                    uint16_t peak3 = 0;
                    peak3 |= extended->getPeekCurrent3().overloadCurrent;
                    peak3 |= extended->getPeekCurrent3().overloadPeriod << 5;
                    peak3 |= extended->getPeekCurrent3().dutyCycle << 11;
                    peak3 |= extended->getPeekCurrent3().vBusVoltageDroop ? 0x8000 : 0;

                    out->writeField('C', (uint64_t) ((((uint64_t) peak3) << 32) | (((uint64_t) peak2) << 16) | peak1));

                    if (extended->getSourceInputs().externalSupplyPresent) {
                        out->writeField('F', (uint8_t) (extended->getSourceInputs().externalSupplyUnconstrained ? 1 : 0));
                    }
                    out->writeField('G',
                            (uint8_t) ((extended->getTouchCurrent().lowTouchCurrentEPS ? 0x1 : 0)
                                    | (extended->getTouchCurrent().groundPinSupported ? 0x2 : 0)
                                    | (extended->getTouchCurrent().groundPinIntendedForEarth ? 0x4 : 0)));
                    out->writeField('T', (uint8_t) extended->getTouchTemp());

                    if (extended->getSourceInputs().internalBatteryPresent) {
                        out->writeField('B', (uint8_t) ((extended->getNumberOfHotswappableBatteries() << 8) | extended->getNumberOfFixedBatteries()));
                    }
                    out->writeField('P', (uint8_t) extended->getSourcePdpRating());

                } else if (Ucpd::getManufacturerInfoMessage()->isValid()) {
                    out->writeField('I',
                            (uint32_t) ((((uint32_t) Ucpd::getManufacturerInfoMessage()->getVID()) << 16)
                                    | Ucpd::getManufacturerInfoMessage()->getPID()));
                }
                const UcpdSourceCapabilitiesMessage *sourceCap = Ucpd::getSourceCapMessage();
                if (sourceCap->isUsbCommunicationsCapable()) {
                    out->writeField('A', (uint8_t) 0);
                }
                if (sourceCap->hasUnconstrainedPower()) {
                    out->writeField('U', (uint8_t) 0);
                }
                if (sourceCap->isUsbSuspendRequired()) {
                    out->writeField('O', (uint8_t) 0);
                }
                if (Ucpd::getManufacturerInfoMessage()->isValid()) {
                    out->writeBigStringField(Ucpd::getManufacturerInfoMessage()->getData(),
                            Ucpd::getManufacturerInfoMessage()->getDataLength());
                }

            }
        }
        out->writeField('N', (uint8_t) Ucpd::getSourceCapMessage()->getPdoNum());

        slot->setComplete(true);
    }

    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code[0];
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 2 && code[0] == RCodeUcpdSourceCommand::code[0] && code[1] == RCodeUcpdSourceCommand::code[1];
    }

    uint8_t getCodeLength() const {
        return 2;
    }

    uint8_t const* getFullCode() const {
        return code;
    }
};

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSOURCECOMMAND_HPP_ */

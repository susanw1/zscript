/*
 * RCodeUcpdSourceStatusCommand.hpp
 *
 *  Created on: 8 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSOURCESTATUSCOMMAND_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSOURCESTATUSCOMMAND_HPP_
#include "RCodeUcpdSystem.hpp"
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "RCodeOutStream.hpp"
#include "commands/RCodeCommand.hpp"

class RCodeUcpdSourceStatusCommand: public RCodeCommand<RCodeParameters> {
private:
    const uint8_t code[2] = { 0x11, 0x13 };

public:

    void execute(RCodeCommandSlot<RCodeParameters> *slot, RCodeCommandSequence<RCodeParameters> *sequence, RCodeOutStream<RCodeParameters> *out) {
        if (Ucpd::getStatusMessage()->isValid() || Ucpd::getPPSStatusMessage()->isValid()) {
            out->writeStatus(OK);
            uint8_t temp = 0;
            uint8_t flags = 0;
            if (Ucpd::getPPSStatusMessage()->isValid() && Ucpd::getStatusMessage()->isValid()) {
                const UcpdParsedStatusMessage *status = Ucpd::getStatusMessage();
                const UcpdParsedPPSStatusMessage *ppsStatus = Ucpd::getPPSStatusMessage();
                temp = ppsStatus->getStatusEventFlags().presentTemp;
                if (temp == 0) {
                    temp = status->getTemperatureStatus();
                }
                flags = (status->getEventFlags().OCP ? 0x02 : 0) | (status->getEventFlags().OTP ? 0x04 : 0) | (status->getEventFlags().OVP ? 0x08 : 0)
                        | (status->getEventFlags().CF ? 0x10 : 0) | (ppsStatus->getStatusEventFlags().isInCurrentLimit ? 0x10 : 0);
            } else if (Ucpd::getPPSStatusMessage()->isValid()) {
                temp = Ucpd::getPPSStatusMessage()->getStatusEventFlags().presentTemp;
                flags = (Ucpd::getPPSStatusMessage()->getStatusEventFlags().isInCurrentLimit ? 0x10 : 0);

            } else if (Ucpd::getStatusMessage()->isValid()) {
                const UcpdParsedStatusMessage *status = Ucpd::getStatusMessage();
                temp = status->getTemperatureStatus();
                flags = (status->getEventFlags().OCP ? 0x02 : 0) | (status->getEventFlags().OTP ? 0x04 : 0) | (status->getEventFlags().OVP ? 0x08 : 0)
                        | (status->getEventFlags().CF ? 0x10 : 0);
            }
            if (temp != 0) {
                out->writeField('H', (uint8_t) temp);
            }
            if (flags != 0) {
                out->writeField('F', (uint8_t) flags);
            }
            if (Ucpd::getStatusMessage()->isValid()) {
                const UcpdParsedStatusMessage *status = Ucpd::getStatusMessage();
                if (status->getInternalTemperature() == 1) {
                    out->writeField('T', (uint8_t) 0);
                } else if (status->getInternalTemperature() > 1) {
                    out->writeField('T', (uint8_t) status->getInternalTemperature());
                }
                uint8_t powerLimitingFactor = 0;
                if (status->getPowerStatus().limitedByCable) {
                    powerLimitingFactor |= 0x02;
                }
                if (status->getPowerStatus().limitedByOtherPorts) {
                    powerLimitingFactor |= 0x04;
                }
                if (status->getPowerStatus().limitedByExternalPower) {
                    powerLimitingFactor |= 0x08;
                }
                if (status->getPowerStatus().limitedByEvents) {
                    powerLimitingFactor |= 0x10;
                }
                if (status->getPowerStatus().limitedByTemperatue) {
                    powerLimitingFactor |= 0x20;
                }
                if (powerLimitingFactor != 0) {
                    out->writeField('L', (uint8_t) powerLimitingFactor);
                }
                out->writeField('D', (uint8_t) status->getPowerStateChange().indicator);
                if (status->getPowerInput().externalPower) {
                    out->writeField('A', (uint8_t) (status->getPowerInput().externalAc ? 1 : 0));
                }

                if (status->getPowerInput().internalOtherPower) {
                    out->writeField('G', (uint8_t) 0);
                }

                if (status->getPowerInput().internalBatteryPower) {
                    out->writeField('B', (uint8_t) ((status->getBatteryInput().fixedBattery1 ? 0x01 : 0) | (status->getBatteryInput().fixedBattery2 ? 0x02 : 0)
                            | (status->getBatteryInput().fixedBattery3 ? 0x04 : 0) | (status->getBatteryInput().fixedBattery4 ? 0x08 : 0)
                            | (status->getBatteryInput().hotswappableBattery1 ? 0x10 : 0) | (status->getBatteryInput().hotswappableBattery2 ? 0x20 : 0)
                            | (status->getBatteryInput().hotswappableBattery3 ? 0x40 : 0) | (status->getBatteryInput().hotswappableBattery4 ? 0x80 : 0)));
                }
                if (status->getPowerStateChange().state != UcpdPowerState::UcpdPowerStateNotSupported) {
                    out->writeField('P', (uint8_t) status->getPowerStateChange().state);
                }
            }
            if (Ucpd::getPPSStatusMessage()->isValid()) {
                const UcpdParsedPPSStatusMessage *ppsStatus = Ucpd::getPPSStatusMessage();
                if (ppsStatus->getOutputVoltage() == 0xFFFF) {
                    out->writeField('V', (uint16_t) ppsStatus->getOutputVoltage());
                }
                if (ppsStatus->getOutputCurrent() == 0xFF) {
                    out->writeField('I', (uint16_t) ppsStatus->getOutputCurrent());
                }
            }

        } else {
            out->writeStatus(CMD_FAIL);
            out->writeBigStringField("No status available");
        }
        Ucpd::updateSourceStatus();
        slot->setComplete(true);
    }

    void setLocks(RCodeCommandSlot<RCodeParameters> *slot, RCodeLockSet<RCodeParameters> *locks) const {
    }

    uint8_t getCode() const {
        return code[0];
    }

    bool matchesCode(uint8_t code[], uint8_t length) const {
        return length == 2 && code[0] == RCodeUcpdSourceStatusCommand::code[0] && code[1] == RCodeUcpdSourceStatusCommand::code[1];
    }

    uint8_t getCodeLength() const {
        return 2;
    }

    uint8_t const* getFullCode() const {
        return code;
    }
};

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSOURCESTATUSCOMMAND_HPP_ */

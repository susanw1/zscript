/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSOURCESTATUSCOMMAND_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSOURCESTATUSCOMMAND_HPP_

#include <zcode/modules/ZcodeCommand.hpp>
#include <usbc-pd-ll/lowlevel/Ucpd.hpp>

#define COMMAND_EXISTS_0114 EXISTENCE_MARKER_UTIL

template<class ZP>
class ZcodeUsbcPDSourceStatusCommand: public ZcodeCommand<ZP> {
public:
    static constexpr uint8_t CODE = 0x04;

    static void execute(ZcodeExecutionCommandSlot<ZP> slot) {
        ZcodeOutStream<ZP> *out = slot.getOut();
        const UcpdParsedStatusMessage<typename ZP::LL> *status = Ucpd<typename ZP::LL>::getStatusMessage();
        const UcpdParsedPPSStatusMessage<typename ZP::LL> *ppsStatus = Ucpd<typename ZP::LL>::getPPSStatusMessage();
        if (status->isValid() || ppsStatus->isValid()) {
            out->writeStatus(OK);
            uint8_t temp = 0;
            uint8_t flags = 0;

            if (ppsStatus->isValid() && status->isValid()) {
                temp = ppsStatus->getStatusEventFlags().presentTemp;
                if (temp == 0) {
                    temp = status->getTemperatureStatus();
                }
                flags = (status->getEventFlags().OCP ? 0x02 : 0) | (status->getEventFlags().OTP ? 0x04 : 0) | (status->getEventFlags().OVP ? 0x08 : 0)
                        | (status->getEventFlags().CF ? 0x10 : 0) | (ppsStatus->getStatusEventFlags().isInCurrentLimit ? 0x10 : 0);
            } else if (ppsStatus->isValid()) {
                temp = ppsStatus->getStatusEventFlags().presentTemp;
                flags = (ppsStatus->getStatusEventFlags().isInCurrentLimit ? 0x10 : 0);
            } else if (status->isValid()) {
                temp = status->getTemperatureStatus();
                flags = (status->getEventFlags().OCP ? 0x02 : 0) | (status->getEventFlags().OTP ? 0x04 : 0) | (status->getEventFlags().OVP ? 0x08 : 0)
                        | (status->getEventFlags().CF ? 0x10 : 0);
            }
            if (temp != 0) {
                out->writeField8('H', temp);
            }
            if (flags != 0) {
                out->writeField8('F', flags);
            }
            if (status->isValid()) {
                if (status->getInternalTemperature() == 1) {
                    out->writeField8('T', 0);
                } else if (status->getInternalTemperature() > 1) {
                    out->writeField8('T', status->getInternalTemperature());
                }
                if (status->getPowerStatus().getField() != 0) {
                    out->writeField8('L', status->getPowerStatus().getField());
                }
                out->writeField8('D', status->getPowerStateChange().indicator);
                if (status->getPowerInput().externalPower) {
                    out->writeField8('A', status->getPowerInput().externalAc ? 1 : 0);
                }

                if (status->getPowerInput().internalOtherPower) {
                    out->writeField8('G', 0);
                }

                if (status->getPowerInput().internalBatteryPower) {
                    out->writeField8('B', status->getBatteryInput().getField());
                }
                if (status->getPowerStateChange().state != UcpdPowerState::UcpdPowerStateNotSupported) {
                    out->writeField8('P', status->getPowerStateChange().state);
                }
            }
            if (ppsStatus->isValid()) {
                if (ppsStatus->getOutputVoltage() != 0xFFFF) {
                    out->writeField16('V', ppsStatus->getOutputVoltage());
                }
                if (ppsStatus->getOutputCurrent() != 0xFF) {
                    out->writeField8('I', ppsStatus->getOutputCurrent());
                }
            }
        } else {
            slot.fail(CMD_FAIL, "No status available");
        }
        Ucpd<typename ZP::LL>::updateSourceStatus();
    }

};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_COMMANDS_ZCODEUSBCPDSOURCESTATUSCOMMAND_HPP_ */

/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDPPSSTATUSMESSAGE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDPPSSTATUSMESSAGE_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "specific/UcpdInternal.hpp"
#include "UcpdParsedStatusMessage.hpp"

struct UcpdPPSStatusEventFlags {
    UcpdStatusTemperatureStatus presentTemp :2;
    bool isInCurrentLimit :1;
};

template<class LL>
class UcpdParsedPPSStatusMessage {
    bool valid = false;
    uint16_t outputVoltage = 0;
    uint8_t outputCurrent = 0;
    UcpdPPSStatusEventFlags statusEventFlags;

public:
    UcpdParsedPPSStatusMessage() {

    }
    UcpdParsedPPSStatusMessage(UcpdExtendedMessage<LL> *message) {
        valid = true;
        outputVoltage = message->getData()[1] << 8 | message->getData()[0];
        outputCurrent = message->getData()[2];
        statusEventFlags.presentTemp = (UcpdStatusTemperatureStatus) ((message->getData()[3] >> 1) & 0x03);
        statusEventFlags.isInCurrentLimit = (message->getData()[3] & 0x08) != 0;
    }
    bool isValid() const {
        return valid;
    }
    uint16_t getOutputVoltage() const {
        return outputVoltage;
    }
    uint8_t getOutputCurrent() const {
        return outputCurrent;
    }
    UcpdPPSStatusEventFlags getStatusEventFlags() const {
        return statusEventFlags;
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDPPSSTATUSMESSAGE_HPP_ */

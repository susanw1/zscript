/*
 * UcpdParsedPPSStatusMessage.hpp
 *
 *  Created on: 7 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDPPSSTATUSMESSAGE_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDPPSSTATUSMESSAGE_HPP_
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

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDPPSSTATUSMESSAGE_HPP_ */

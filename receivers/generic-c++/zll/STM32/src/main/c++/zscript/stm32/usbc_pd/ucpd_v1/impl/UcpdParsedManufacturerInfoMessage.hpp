/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDMANUFACTURERINFOMESSAGE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDMANUFACTURERINFOMESSAGE_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "specific/UcpdInternal.hpp"

template<class LL>
class UcpdParsedManufacturerInfoMessage {
    bool valid = false;
    uint16_t VID = 0;
    uint16_t PID = 0;
    uint8_t data[21];
    uint8_t dataLength = 0;

public:
    UcpdParsedManufacturerInfoMessage() {
    }
    UcpdParsedManufacturerInfoMessage(UcpdExtendedMessage<LL> *message) {
        valid = true;
        VID = message->getData()[1] << 8 | message->getData()[0];
        PID = message->getData()[3] << 8 | message->getData()[2];
        while (message->getData()[dataLength + 4] != 0) {
            data[dataLength] = message->getData()[4 + dataLength];
            dataLength++;
        }
    }
    bool isValid() const {
        return valid;
    }
    uint16_t getVID() const {
        return VID;
    }
    uint16_t getPID() const {
        return PID;
    }
    uint8_t getDataLength() const {
        return dataLength;
    }
    const uint8_t* getData() const {
        return data;
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDMANUFACTURERINFOMESSAGE_HPP_ */

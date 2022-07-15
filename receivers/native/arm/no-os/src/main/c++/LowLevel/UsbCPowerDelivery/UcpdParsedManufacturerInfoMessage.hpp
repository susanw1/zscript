/*
 * UcpdParsedManufacturerInfoMessage.hpp
 *
 *  Created on: 7 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDMANUFACTURERINFOMESSAGE_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDMANUFACTURERINFOMESSAGE_HPP_
#include <llIncludes.hpp>
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

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDMANUFACTURERINFOMESSAGE_HPP_ */

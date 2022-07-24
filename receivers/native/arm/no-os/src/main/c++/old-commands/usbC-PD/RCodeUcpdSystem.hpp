/*
 * RCodeUcpdSystem.hpp
 *
 *  Created on: 6 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSYSTEM_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSYSTEM_HPP_
#include "RCodeIncludes.hpp"
#include "RCodeParameters.hpp"
#include "Ucpd.hpp"
#include "UcpdParsedExtendedSourceCapabilitiesMessage.hpp"
#include "UcpdParsedStatusMessage.hpp"
#include "UcpdParsedExtendedSourceCapabilitiesMessage.hpp"
#include "UcpdParsedPPSStatusMessage.hpp"
#include "UcpdParsedManufacturerInfoMessage.hpp"

class Ucpd {
    static PowerStatus status;
    static uint16_t maxCurrent;         // in 10mA
    static uint16_t minVoltage;          // in 50mV
    static uint16_t maxVoltage;          // in 50mV
    static UcpdSourceCapabilitiesMessage sourceCapMessage;

    static UcpdParsedExtendedSourceCapabilitiesMessage extendedSourceCapMessage;
    static UcpdParsedStatusMessage statusMessage;
    static UcpdParsedPPSStatusMessage PPSStatusMessage;
    static UcpdParsedManufacturerInfoMessage manufacturerInfoMessage;

    static void powerStatusChangeHandler(PowerStatus newStatus, PowerContract contract) {
        if (newStatus != ContractMade || status == Negotiating) {
            status = newStatus;
        }
        maxCurrent = contract.getCurrent();
        minVoltage = contract.getMinVoltage();
        maxVoltage = contract.getMaxVoltage();
        if (newStatus == ContractMade) {
            UcpdInternal::sendGetManufacturerInfo();
        }
    }
    static void messageOfInterestHandler(UcpdMessage *message) {
        if (message->getUcpdMessageType() == ExtendedMessage) {
            switch (((UcpdExtendedMessage*) message)->getUcpdExtendedMessageType()) {
            case PPSStatus:
                PPSStatusMessage = UcpdParsedPPSStatusMessage((UcpdExtendedMessage*) message);
                break;
            case ManufacturerInfo:
                UcpdInternal::sendGetStatus();
                manufacturerInfoMessage = UcpdParsedManufacturerInfoMessage((UcpdExtendedMessage*) message);
                break;
            case Status:
                UcpdInternal::sendGetSourceCapExtended();
                statusMessage = UcpdParsedStatusMessage((UcpdExtendedMessage*) message);
                break;
            case SourceCapabilitiesExtended:
                UcpdInternal::sendGetPPSStatus();
                extendedSourceCapMessage = UcpdParsedExtendedSourceCapabilitiesMessage((UcpdExtendedMessage*) message);
                break;
            default:
                break;
            }
        } else if (message->getUcpdMessageType() == DataMessage && ((UcpdDataMessage*) message)->getUcpdDataMessageType() == SourceCapabilities) {
            sourceCapMessage = *((UcpdSourceCapabilitiesMessage*) message);
        }
    }

public:
    static void init(uint32_t *unInitialisedStore, uint16_t maxCurrent, uint16_t minVoltage, uint16_t maxVoltage) {
        UcpdInternal::setPowerStatusChangeCallback(&Ucpd::powerStatusChangeHandler);
        UcpdInternal::setMessageOfInterestCallback(&Ucpd::messageOfInterestHandler);
        UcpdInternal::init(unInitialisedStore, maxCurrent, minVoltage, maxVoltage, HighestPower10DefaultLow);
    }

    static PowerStatus getPowerStatus() {
        return status;
    }

    static uint16_t getMaxCurrent() {
        return maxCurrent;
    }

    static uint16_t getMinVoltage() {
        return minVoltage;
    }

    static uint16_t getMaxVoltage() {
        return maxVoltage;
    }

    static void renegotiateAll(uint16_t maxCurrent, uint16_t minVoltage, uint16_t maxVoltage) {
        status = Negotiating;
        UcpdInternal::changeAll(maxCurrent, minVoltage, maxVoltage, HighestPower10DefaultLow);
    }

    static void renegotiateVoltage(uint16_t minVoltage, uint16_t maxVoltage) {
        status = Negotiating;
        UcpdInternal::changeTargetVoltage(minVoltage, maxVoltage, HighestPower10DefaultLow);
    }

    static void renegotiateCurrent(uint16_t maxCurrent) {
        status = Negotiating;
        UcpdInternal::setTargetCurrent(maxCurrent);
    }

    static void updateSourceStatus() {
        UcpdInternal::sendGetStatus();
    }

    static void tick() {
        UcpdInternal::tick();
    }

    static const UcpdSourceCapabilitiesMessage* getSourceCapMessage() {
        return &sourceCapMessage;
    }
    static const UcpdParsedExtendedSourceCapabilitiesMessage* getExtendedSourceCapMessage() {
        return &extendedSourceCapMessage;
    }
    static const UcpdParsedStatusMessage* getStatusMessage() {
        return &statusMessage;
    }
    static const UcpdParsedPPSStatusMessage* getPPSStatusMessage() {
        return &PPSStatusMessage;
    }
    static const UcpdParsedManufacturerInfoMessage* getManufacturerInfoMessage() {
        return &manufacturerInfoMessage;
    }
};

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_RCODEUCPDSYSTEM_HPP_ */

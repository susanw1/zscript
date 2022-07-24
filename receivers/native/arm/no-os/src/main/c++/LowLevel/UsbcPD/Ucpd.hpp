/*
 * RCodeUcpdSystem.hpp
 *
 *  Created on: 6 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_UCPD_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_UCPD_HPP_
#include <LowLevel/llIncludes.hpp>
#include "specific/UcpdInternal.hpp"
#include "UcpdParsedExtendedSourceCapabilitiesMessage.hpp"
#include "UcpdParsedManufacturerInfoMessage.hpp"
#include "UcpdParsedPPSStatusMessage.hpp"
#include "UcpdParsedStatusMessage.hpp"

template<class LL>
class Ucpd {
    static PowerStatus status;
    static uint16_t maxCurrent;         // in 10mA
    static uint16_t minVoltage;          // in 50mV
    static uint16_t maxVoltage;          // in 50mV
    static UcpdSourceCapabilitiesMessage<LL> sourceCapMessage;

    static UcpdParsedExtendedSourceCapabilitiesMessage<LL> extendedSourceCapMessage;
    static UcpdParsedStatusMessage<LL> statusMessage;
    static UcpdParsedPPSStatusMessage<LL> PPSStatusMessage;
    static UcpdParsedManufacturerInfoMessage<LL> manufacturerInfoMessage;

    static void powerStatusChangeHandler(PowerStatus newStatus, PowerContract contract) {
        if (newStatus != ContractMade || status == Negotiating) {
            status = newStatus;
        }
        maxCurrent = contract.getCurrent();
        minVoltage = contract.getMinVoltage();
        maxVoltage = contract.getMaxVoltage();
        if (newStatus == ContractMade) {
            UcpdInternal<LL>::sendGetManufacturerInfo();
        }
    }
    static void messageOfInterestHandler(UcpdMessage<LL> *message) {
        if (message->getUcpdMessageType() == ExtendedMessage) {
            switch (((UcpdExtendedMessage<LL>*) message)->getUcpdExtendedMessageType()) {
            case PPSStatus:
                PPSStatusMessage = UcpdParsedPPSStatusMessage<LL>((UcpdExtendedMessage<LL>*) message);
                break;
            case ManufacturerInfo:
                UcpdInternal<LL>::sendGetStatus();
                manufacturerInfoMessage = UcpdParsedManufacturerInfoMessage<LL>((UcpdExtendedMessage<LL>*) message);
                break;
            case Status:
                UcpdInternal<LL>::sendGetSourceCapExtended();
                statusMessage = UcpdParsedStatusMessage<LL>((UcpdExtendedMessage<LL>*) message);
                break;
            case SourceCapabilitiesExtended:
                UcpdInternal<LL>::sendGetPPSStatus();
                extendedSourceCapMessage = UcpdParsedExtendedSourceCapabilitiesMessage<LL>((UcpdExtendedMessage<LL>*) message);
                break;
            default:
                break;
            }
        } else if (message->getUcpdMessageType() == DataMessage && ((UcpdDataMessage<LL>*) message)->getUcpdDataMessageType() == SourceCapabilities) {
            sourceCapMessage = *((UcpdSourceCapabilitiesMessage<LL>*) message);
        }
    }

public:
    static void init(uint32_t *unInitialisedStore, uint16_t maxCurrent, uint16_t minVoltage, uint16_t maxVoltage) {
        UcpdInternal<LL>::setPowerStatusChangeCallback(&Ucpd::powerStatusChangeHandler);
        UcpdInternal<LL>::setMessageOfInterestCallback(&Ucpd::messageOfInterestHandler);
        UcpdInternal<LL>::init(unInitialisedStore, maxCurrent, minVoltage, maxVoltage, HighestPower10DefaultLow);
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
        UcpdInternal<LL>::changeAll(maxCurrent, minVoltage, maxVoltage, HighestPower10DefaultLow);
    }

    static void renegotiateVoltage(uint16_t minVoltage, uint16_t maxVoltage) {
        status = Negotiating;
        UcpdInternal<LL>::changeTargetVoltage(minVoltage, maxVoltage, HighestPower10DefaultLow);
    }

    static void renegotiateCurrent(uint16_t maxCurrent) {
        status = Negotiating;
        UcpdInternal<LL>::setTargetCurrent(maxCurrent);
    }

    static void updateSourceStatus() {
        UcpdInternal<LL>::sendGetStatus();
    }

    static void tick() {
        UcpdInternal<LL>::tick();
    }

    static const UcpdSourceCapabilitiesMessage<LL>* getSourceCapMessage() {
        return &sourceCapMessage;
    }
    static const UcpdParsedExtendedSourceCapabilitiesMessage<LL>* getExtendedSourceCapMessage() {
        return &extendedSourceCapMessage;
    }
    static const UcpdParsedStatusMessage<LL>* getStatusMessage() {
        return &statusMessage;
    }
    static const UcpdParsedPPSStatusMessage<LL>* getPPSStatusMessage() {
        return &PPSStatusMessage;
    }
    static const UcpdParsedManufacturerInfoMessage<LL>* getManufacturerInfoMessage() {
        return &manufacturerInfoMessage;
    }
};
template<class LL>
PowerStatus Ucpd<LL>::status = Negotiating;
template<class LL>
uint16_t Ucpd<LL>::maxCurrent = 15;         // in 10mA
template<class LL>
uint16_t Ucpd<LL>::minVoltage = 100;          // in 50mV
template<class LL>
uint16_t Ucpd<LL>::maxVoltage = 100;          // in 50mV

template<class LL>
UcpdSourceCapabilitiesMessage<LL> Ucpd<LL>::sourceCapMessage;
template<class LL>
UcpdParsedExtendedSourceCapabilitiesMessage<LL> Ucpd<LL>::extendedSourceCapMessage;
template<class LL>
UcpdParsedStatusMessage<LL> Ucpd<LL>::statusMessage;
template<class LL>
UcpdParsedPPSStatusMessage<LL> Ucpd<LL>::PPSStatusMessage;
template<class LL>
UcpdParsedManufacturerInfoMessage<LL> Ucpd<LL>::manufacturerInfoMessage;

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_UCPD_HPP_ */

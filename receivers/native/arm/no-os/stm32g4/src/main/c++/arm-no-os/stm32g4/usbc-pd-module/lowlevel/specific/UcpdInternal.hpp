/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDINTERNAL_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDINTERNAL_HPP_

#include <arm-no-os/llIncludes.hpp>
#include <arm-no-os/system/clock/SystemMilliClock.hpp>
#include <arm-no-os/system/clock/ClockManager.hpp>
#include <arm-no-os/system/dma/DmaManager.hpp>
#include "UcpdParser.hpp"

enum PowerStatus {
    Negotiating,
    ContractMade,                   // has a contract - not nessesarily the right one
    UnresponsiveSource,             // defaulting to 5v, talking to a resistor
    SourceCannotProvideVoltage,     // source cannot provide a voltage in range
    NewVoltageContractMade,         // voltage contract changed - power needs satisfied
    NewVoltageInsufficientCurrent,  // voltage contract changed - power needs not satisfied
    SourceCannotProvideCurrent,     // just changed current, not enough for stated requirements
    HardResetImminent,
    OverCurrentAlert,
    OverTemperatureAlert,
    OverVoltageAlert
};
enum VoltageNegotiationStrategy {
    HighestPower,   // defaults to higher voltage if same
    HighestCurrent,   // defaults to higher voltage if same
    LowestVoltage,
    HighestVoltage,
    HighestPower10DefaultLow, // if within 10% of larger power, use lower voltage
    HighestPower20DefaultLow, // if within 20% of larger power, use lower voltage
};
class PowerContract {
    uint16_t minVoltage;
    uint16_t maxVoltage;
    uint16_t current;

public:
    PowerContract(uint16_t minVoltage, uint16_t maxVoltage, uint16_t current) :
            minVoltage(minVoltage), maxVoltage(maxVoltage), current(current) {
    }
    uint16_t getMinVoltage() {
        return minVoltage;
    }
    uint16_t getMaxVoltage() {
        return maxVoltage;
    }
    uint16_t getCurrent() {
        return current;
    }
    uint16_t getPower() {
        return ((uint32_t) (current / 10) * (uint32_t) (minVoltage / 10)) / 5;
    }
};

// voltages in 50mV units
// currents in 10mA units
// powers in 250mW units
template<class LL>
class UcpdInternal {

    static void doNotUseMessage(UcpdMessage<LL> *m) {
        (void) m;
    }

    static volatile uint32_t expectResponseTime;
    static volatile uint32_t resendTime;
    static volatile uint32_t hardResetAt;
    static volatile bool hardResetTiming;
    static volatile bool resendRequestTiming;
    static volatile bool expectResponse;

    static bool useChunking;

    static void (*powerStatusChangeCallback)(PowerStatus, PowerContract);
    static void (*messageOfInterestCallback)(UcpdMessage<LL>*);

    static uint32_t *unInitialisedStore;

    static uint16_t absMinVoltage;
    static uint16_t absMaxVoltage;
    static VoltageNegotiationStrategy strategy;

    static PowerContract currentContract;

    static PowerContract requestedContract;

    static uint16_t currentContractMaxCurrent;
    static uint8_t currentContractObjectPosition;

    static uint16_t targetAbsMinVoltage;
    static uint16_t targetAbsMaxVoltage;

    static uint16_t targetCurrent;

    static uint8_t hardResetCount;
    static bool hasAttemptedSoftReset;
    static bool hasSentHardReset;

    static bool voltageMismatch;

    static bool hasContract;

    static void hardResetHandler() {
        *unInitialisedStore = hardResetCount | 0x80000000;
        hardResetTiming = true;
        powerStatusChangeCallback(HardResetImminent, currentContract);
        hardResetAt = SystemMilliClock<LL>::getTimeMillis() + 500;
    }
    static void messageSentHandler(UcpdMessageSentStatus status) {
        if (status != GoodCrcReceived) {
            if (status != SoftResetFailed && !hasAttemptedSoftReset) {
                hasAttemptedSoftReset = true;
                UcpdParser<LL>::sendSoftReset();
                UcpdParser<LL>::softReset();
            } else {
                hasAttemptedSoftReset = false;
                UcpdParser<LL>::sendHardReset();
                resendRequestTiming = false;
                expectResponse = false;
                hardResetAt = SystemMilliClock<LL>::getTimeMillis() + 500;
                hardResetTiming = true;
            }
        }
    }
    static void messageReceivedHandler(UcpdMessage<LL> *message) {
        if (message->getUcpdMessageType() == ExtendedMessage) {
            UcpdExtendedMessage<LL> *extendedMessage = (UcpdExtendedMessage<LL>*) message;
            if (useChunking) {
                UcpdControlMessage<LL> message(NotSupported);
                UcpdParser<LL>::sendMessage(&message);
            } else {
                if (extendedMessage->getUcpdExtendedMessageType() == GetManufacturerInfo) {
                    sendManufacturerInfo();
                } else if (extendedMessage->getUcpdExtendedMessageType() == ManufacturerInfo
                        || extendedMessage->getUcpdExtendedMessageType() == SourceCapabilitiesExtended
                        || extendedMessage->getUcpdExtendedMessageType() == PPSStatus
                        || extendedMessage->getUcpdExtendedMessageType() == Status) {
                    messageOfInterestCallback(message);
                } else {
                    UcpdControlMessage<LL> message(NotSupported);
                    UcpdParser<LL>::sendMessage(&message);
                }
            }
        } else if (message->getUcpdMessageType() == ControlMessage) {
            UcpdControlMessage<LL> *controlMessage = (UcpdControlMessage<LL>*) message;
            switch (controlMessage->getUcpdControlMessageType()) {
            case Accept:
                if (hasAttemptedSoftReset) {
                    hasAttemptedSoftReset = false;
                    UcpdParser<LL>::resendCurrent();
                } else {
                    currentContract = requestedContract;
                    hardResetAt = SystemMilliClock<LL>::getTimeMillis() + 500;
                    hardResetTiming = true;
                }
                break;
            case Reject:
                powerStatusChangeCallback(SourceCannotProvideVoltage, currentContract);
                break;
            case PSRdy:
                hasContract = true;
                hardResetTiming = false;
                powerStatusChangeCallback(ContractMade, currentContract);
                hardResetCount = 0;
                break;
            case GetSinkCap:
                sendSinkCap();
                break;
            case SoftReset:
                {
                hasAttemptedSoftReset = false;
                UcpdControlMessage<LL> accept(Accept);
                UcpdParser<LL>::sendMessage(&accept);
            }
                break;
            case GetSinkCapExtended:
                sendSinkCapExtended();
                break;
            case Wait:
                resendTime = SystemMilliClock<LL>::getTimeMillis() + 100;
                resendRequestTiming = true;
                break;
            case NotSupported:
                break;
            default:
                {
                UcpdControlMessage<LL> ns(NotSupported);
                UcpdParser<LL>::sendMessage(&ns);
            }
                break;
            }
        } else {
            UcpdDataMessage<LL> *dataMessage = (UcpdDataMessage<LL>*) message;
            switch (dataMessage->getUcpdDataMessageType()) {
            case SourceCapabilities:
                messageOfInterestCallback(message);
                useChunking = !((UcpdSourceCapabilitiesMessage<LL>*) dataMessage)->hasUnchunkedExtendedMessageSupport();
                hardResetCount = 0;
                hasAttemptedSoftReset = false;
                expectResponse = false;
                hardResetTiming = false;
                choosePowerContract((UcpdSourceCapabilitiesMessage<LL>*) dataMessage);
                break;
            case Alert:
                {
                messageOfInterestCallback(message);
                UcpdAlertMessage<LL> *alertMessage = (UcpdAlertMessage<LL>*) dataMessage;
                if (alertMessage->hasOcpEvent()) {
                    powerStatusChangeCallback(OverCurrentAlert, currentContract);
                }
                if (alertMessage->hasOvpEvent()) {
                    powerStatusChangeCallback(OverVoltageAlert, currentContract);
                }
                if (alertMessage->hasOtpEvent()) {
                    powerStatusChangeCallback(OverTemperatureAlert, currentContract);
                }
                sendGetStatus();
            }
                break;
            case GetCountryInfo:
                case Request:
                case BIST:
                case SinkCapabilities:
                case BatteryStatus:
                case EnterUSB:
                {
                UcpdControlMessage<LL> ns(NotSupported);
                UcpdParser<LL>::sendMessage(&ns);
            }
                break;
            default:
                break;
            }
        }
    }
    static void sendSinkCap() {
        UcpdSinkCapabilitiesMessage<LL> message(false, true, false, true, false);
        message.addPDO(PowerDataObject<LL>(5 * 20, 5 * 20, targetCurrent)); //need a vSafe5v PDO
        if (absMaxVoltage > 5 * 20 && absMinVoltage < 5 * 20) { //check for a good 5v PDO
            message.addPDO(PowerDataObject<LL>(5 * 20, 5 * 20, targetCurrent));
        }
        if (absMaxVoltage > 9 * 20 && absMinVoltage < 9 * 20) { //check for a good 9v PDO
            message.addPDO(PowerDataObject<LL>(9 * 20, 9 * 20, targetCurrent));
        }
        if (absMaxVoltage > 15 * 20 && absMinVoltage < 15 * 20) { //check for a good 15v PDO
            message.addPDO(PowerDataObject<LL>(15 * 20, 15 * 20, targetCurrent));
        }
        if (absMaxVoltage > 20 * 20 && absMinVoltage < 20 * 20) { //check for a good 20v PDO
            message.addPDO(PowerDataObject<LL>(20 * 20, 20 * 20, targetCurrent));
        }
        message.addPDO(PowerDataObject<LL>(absMaxVoltage, absMinVoltage, targetCurrent)); //check for a good variable power supply
        UcpdParser<LL>::sendMessage(&message);
    }
    static void sendSinkCapExtended() {
        uint16_t vidTmp = LL::ucpdVID;
        uint16_t pidTmp = LL::ucpdPID;
        uint32_t xidTmp = LL::ucpdXID;
        uint8_t *vid = (uint8_t*) &vidTmp;
        uint8_t *pid = (uint8_t*) &pidTmp;
        uint8_t *xid = (uint8_t*) &xidTmp;
        uint8_t data[21] = { vid[findBytePairInvertedAddress<LL>(0)], vid[findBytePairInvertedAddress<LL>(1)],
                pid[findBytePairInvertedAddress<LL>(0)], pid[findBytePairInvertedAddress<LL>(1)],
                xid[findDataObjectInvertedAddress<LL>(0)], xid[findDataObjectInvertedAddress<LL>(1)],
                xid[findDataObjectInvertedAddress<LL>(2)], xid[findDataObjectInvertedAddress<LL>(3)],
                // assumes no overload, gives lots of load step (500mA/us as opposed to 150mA/us)
                LL::ucpdFirmwareRevisionNum, LL::ucpdHardwareRevisionNum, 1, 0, 0x00, 0x00, 0, 0, 0, 0x02, // the 2 means it is VBus Powered
                LL::ucpdMinPower, LL::ucpdOperationalPower, LL::ucpdMaxPower };
        UcpdExtendedMessage<LL> message(SinkCapabilitiesExtended, useChunking, 0, false, 21, data);
        UcpdParser<LL>::sendMessage(&message);
    }
    static void sendManufacturerInfo() {
        int i = 0;
        for (; LL::ucpdManufacturerInfo[i] != 0; i++)
            ;
        UcpdExtendedMessage<LL> message(ManufacturerInfo, false, 0, false, i, (const uint8_t*) LL::ucpdManufacturerInfo);
        UcpdParser<LL>::sendMessage(&message);
    }

    static void choosePowerContract(UcpdSourceCapabilitiesMessage<LL> *cap) {
        uint8_t validOptions[7];
        uint8_t optionNum = 0;
        for (uint8_t i = 0; i < cap->getPdoNum(); ++i) {
            PowerDataObject<LL> obj = cap->getPdos()[i];
            if (obj.getMaximumVoltage() <= targetAbsMaxVoltage && obj.getMinimumVoltage() >= targetAbsMinVoltage) {
                validOptions[optionNum++] = i;
            }
        }
        uint8_t winner;
        PowerStatus endStatus = NewVoltageContractMade;
        if (optionNum == 0) {
            winner = 0;
            endStatus = SourceCannotProvideVoltage;
        } else if (optionNum == 1) {
            winner = validOptions[0];
        } else if (strategy == HighestPower) {
            uint32_t highestPower = 0;
            int currentBest = 0;
            for (int i = 0; i < optionNum; ++i) {
                PowerDataObject<LL> obj = cap->getPdos()[i];
                if (obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent() >= highestPower) {
                    currentBest = i;
                    highestPower = obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent();
                }
            }
            winner = validOptions[currentBest];
        } else if (strategy == HighestCurrent) {
            uint16_t highestCurrent = 0;
            int currentBest = 0;
            for (int i = 0; i < optionNum; ++i) {
                PowerDataObject<LL> obj = cap->getPdos()[i];
                if (obj.getMaximumCurrent() >= highestCurrent) {
                    currentBest = i;
                    highestCurrent = obj.getMaximumCurrent();
                }
            }
            winner = validOptions[currentBest];
        } else if (strategy == LowestVoltage) {
            uint16_t lowestVoltage = 0;
            int currentBest = 0;
            for (int i = 0; i < optionNum; ++i) {
                PowerDataObject<LL> obj = cap->getPdos()[i];
                if (obj.getMaximumVoltage() < lowestVoltage) {
                    currentBest = i;
                    lowestVoltage = obj.getMaximumVoltage();
                }
            }
            winner = validOptions[currentBest];
        } else if (strategy == HighestVoltage) {
            uint16_t highestVoltage = 0;
            int currentBest = 0;
            for (int i = 0; i < optionNum; ++i) {
                PowerDataObject<LL> obj = cap->getPdos()[i];
                if (obj.getMinimumVoltage() >= highestVoltage) {
                    currentBest = i;
                    highestVoltage = obj.getMinimumVoltage();
                }
            }
            winner = validOptions[currentBest];
        } else if (strategy == HighestPower10DefaultLow) {
            uint32_t highestPower = 0;
            for (int i = 0; i < optionNum; ++i) {
                PowerDataObject<LL> obj = cap->getPdos()[i];
                if (obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent() >= highestPower) {
                    highestPower = obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent();
                }
            }
            uint16_t currentVoltage = 0xFFFF;
            int currentBest = 0;
            for (int i = 0; i < optionNum; ++i) {
                PowerDataObject<LL> obj = cap->getPdos()[i];
                if (obj.getMaximumVoltage() > currentVoltage) {
                    if (obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent() * 10 >= highestPower * 11) {
                        currentBest = i;
                    }
                } else {
                    if (obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent() * 11 >= highestPower * 10) {
                        currentBest = i;
                    }
                }
            }
            winner = validOptions[currentBest];
        } else if (strategy == HighestPower20DefaultLow) {
            uint32_t highestPower = 0;
            for (int i = 0; i < optionNum; ++i) {
                PowerDataObject<LL> obj = cap->getPdos()[i];
                if (obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent() >= highestPower) {
                    highestPower = obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent();
                }
            }
            uint16_t currentVoltage = 0xFFFF;
            int currentBest = 0;
            for (int i = 0; i < optionNum; ++i) {
                PowerDataObject<LL> obj = cap->getPdos()[i];
                if (obj.getMaximumVoltage() > currentVoltage) {
                    if (obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent() * 10 >= highestPower * 12) {
                        currentBest = i;
                    }
                } else {
                    if (obj.getMinimumVoltage() * (uint32_t) obj.getMaximumCurrent() * 12 >= highestPower * 10) {
                        currentBest = i;
                    }
                }
            }
            winner = validOptions[currentBest];
        } else {
            winner = 0;
        }
        currentContractObjectPosition = winner;
        bool hasMismatch;
        if (endStatus == NewVoltageContractMade) {
            voltageMismatch = false;
            PowerDataObject<LL> selected = cap->getPdos()[winner];
            if (selected.getMaximumCurrent() < targetCurrent) {
                endStatus = NewVoltageInsufficientCurrent;
                requestedContract = PowerContract(selected.getMinimumVoltage(), selected.getMaximumVoltage(), selected.getMaximumCurrent());
                hasMismatch = true;
            } else {
                hasMismatch = false;
                requestedContract = PowerContract(selected.getMinimumVoltage(), selected.getMaximumVoltage(), targetCurrent);
            }
        } else {
            hasMismatch = true;
            voltageMismatch = true;
            if (cap->getPdos()[0].getMaximumCurrent() > 100) {
                requestedContract = PowerContract(5 * 20, 5 * 20, 100);
            } else {
                requestedContract = PowerContract(5 * 20, 5 * 20, cap->getPdos()[0].getMaximumCurrent());
            }
        }
        currentContractMaxCurrent = cap->getPdos()[winner].getMaximumCurrent();
        UcpdRequestMessage<LL> request(currentContractObjectPosition, false, hasMismatch, true, true,
                true, requestedContract.getCurrent(), requestedContract.getCurrent());
        UcpdParser<LL>::sendMessage(&request);
        absMaxVoltage = targetAbsMaxVoltage;
        absMinVoltage = targetAbsMinVoltage;
        powerStatusChangeCallback(endStatus, currentContract);
    }

public:
    static void init(uint32_t *unInitialisedStore, uint16_t targetCurrent, uint16_t minVoltage, uint16_t maxVoltage, VoltageNegotiationStrategy strategy) {
        UcpdInternal::hasContract = false;
        UcpdInternal::unInitialisedStore = unInitialisedStore;
        UcpdInternal::absMinVoltage = minVoltage;
        UcpdInternal::absMaxVoltage = maxVoltage;
        UcpdInternal::targetAbsMaxVoltage = maxVoltage;
        UcpdInternal::targetAbsMinVoltage = minVoltage;
        UcpdInternal::strategy = strategy;
        UcpdInternal::currentContract = PowerContract(5 * 20, 5 * 20, 15);
        UcpdInternal::requestedContract = currentContract;
        UcpdInternal::currentContractMaxCurrent = 15;
        UcpdInternal::currentContractObjectPosition = 0;
        UcpdInternal::targetCurrent = targetCurrent;
        if ((*unInitialisedStore & 0xC0000000) == 0x80000000) {
            UcpdInternal::hardResetCount = (*unInitialisedStore) & 0xF;
        } else {
            UcpdInternal::hardResetCount = 0;
        }
        UcpdInternal::hardResetAt = SystemMilliClock<LL>::getTimeMillis() + 500;
        UcpdInternal::expectResponseTime = 0;
        UcpdInternal::resendTime = 0;
        UcpdInternal::hardResetTiming = true;
        UcpdInternal::resendRequestTiming = false;
        UcpdInternal::hasAttemptedSoftReset = false;
        UcpdInternal::hasSentHardReset = false;
        UcpdInternal::expectResponse = false;
        UcpdInternal::voltageMismatch = false;
        UcpdParser<LL>::setMessageReceivedCallback(messageReceivedHandler);
        UcpdParser<LL>::setHardResetCallback(hardResetHandler);
        UcpdParser<LL>::setMessageSentCallback(messageSentHandler);
        UcpdParser<LL>::init();
        if (hardResetCount > 2) {
            hasContract = true;
        }
        while (!hasContract && hardResetCount <= 2) {
            tick();
        }
//        UcpdControlMessage<LL> control(GotoMin);
//        while (!UcpdParser<LL>::sendMessage(&control))
//            ;
    }
    static void setPowerStatusChangeCallback(void (*powerStatusChangeCallback)(PowerStatus, PowerContract)) {
        UcpdInternal::powerStatusChangeCallback = powerStatusChangeCallback;
    }
    static void setMessageOfInterestCallback(void (*messageOfInterestCallback)(UcpdMessage<LL>*)) {
        // gets ManufacturerInfo, SourceCapabilitiesExtended, PPSStatus, Status, SourceCapabilities messages - needs to be quick
        UcpdInternal::messageOfInterestCallback = messageOfInterestCallback;
    }
    static void setTargetPower(uint16_t power) {
        setTargetCurrent((((uint32_t) power) * 500) / currentContract.getMinVoltage());
    }

    static void setTargetCurrent(uint16_t current) {
        bool hasMismatch = false;
        uint16_t currentToRequest = current;
        targetCurrent = current;
        if (!voltageMismatch) {
            if (current > currentContractMaxCurrent) {
                hasMismatch = true;
                currentToRequest = currentContractMaxCurrent;
            }
            requestedContract = PowerContract(currentContract.getMinVoltage(), currentContract.getMaxVoltage(), currentToRequest);
            UcpdRequestMessage<LL> request(currentContractObjectPosition, false, hasMismatch && voltageMismatch, true, true,
                    true, currentToRequest, currentToRequest);
            UcpdParser<LL>::sendMessage(&request);
        }
    }

    static void changeTargetVoltage(uint16_t minVoltage, uint16_t maxVoltage, VoltageNegotiationStrategy strategy) {
        UcpdInternal::strategy = strategy;
        targetCurrent = currentContract.getCurrent();
        targetAbsMaxVoltage = maxVoltage;
        targetAbsMinVoltage = minVoltage;
        expectResponseTime = SystemMilliClock<LL>::getTimeMillis() + 31;
        expectResponse = true;
        UcpdControlMessage<LL> message(GetSourceCap);
        UcpdParser<LL>::sendMessage(&message);
    }

    static void changeAll(uint16_t current, uint16_t minVoltage, uint16_t maxVoltage, VoltageNegotiationStrategy strategy) {
        UcpdInternal::strategy = strategy;
        targetCurrent = current;
        targetAbsMaxVoltage = maxVoltage;
        targetAbsMinVoltage = minVoltage;
        expectResponseTime = SystemMilliClock<LL>::getTimeMillis() + 31;
        expectResponse = true;
        UcpdControlMessage<LL> message(GetSourceCap);
        UcpdParser<LL>::sendMessage(&message);
    }

    static void renegotiateNoChange() {
        expectResponseTime = SystemMilliClock<LL>::getTimeMillis() + 31;
        expectResponse = true;
        UcpdControlMessage<LL> message(GetSourceCap);
        UcpdParser<LL>::sendMessage(&message);
    }

    static bool sendGetManufacturerInfo() {
        if (!useChunking) {
            uint8_t data[2] = { 0, 0 };
            UcpdExtendedMessage<LL> message(GetManufacturerInfo, false, 0, false, 2, data);
            UcpdParser<LL>::sendMessage(&message);
        }
        return !useChunking;
    }

    static bool sendGetSourceCapExtended() {
        if (!useChunking) {
            UcpdControlMessage<LL> message(GetSourceCapExtended);
            UcpdParser<LL>::sendMessage(&message);
        }
        return !useChunking;
    }
    static bool sendGetStatus() {
        if (!useChunking) {
            UcpdControlMessage<LL> message(GetStatus);
            UcpdParser<LL>::sendMessage(&message);
        }
        return !useChunking;
    }
    static bool sendGetPPSStatus() {
        if (!useChunking) {
            UcpdControlMessage<LL> message(GetPPSStatus);
            UcpdParser<LL>::sendMessage(&message);
        }
        return !useChunking;
    }

    static void tick() {
        UcpdParser<LL>::tick();
        if (hardResetTiming) {
            if (SystemMilliClock<LL>::getTimeMillis() >= hardResetAt) {
                if (hardResetCount <= 2) {
                    UcpdParser<LL>::sendHardReset();
                    hardResetAt = SystemMilliClock<LL>::getTimeMillis() + 500;
                    hardResetCount++;
                } else {
                    hardResetTiming = false;
                    powerStatusChangeCallback(UnresponsiveSource, currentContract);
                }
            }
        }
        if (resendRequestTiming) {
            if (SystemMilliClock<LL>::getTimeMillis() >= resendTime) {
                resendRequestTiming = false;
                bool hasMismatch = false;
                if (targetCurrent > currentContractMaxCurrent) {
                    hasMismatch = true;
                }
                UcpdRequestMessage<LL> request(currentContractObjectPosition, false, hasMismatch && voltageMismatch, true, true,
                        true, requestedContract.getCurrent(), requestedContract.getCurrent());
                UcpdParser<LL>::sendMessage(&request);
            }
        }
        if (expectResponse) {
            if (SystemMilliClock<LL>::getTimeMillis() >= expectResponseTime) {
                UcpdParser<LL>::softReset();
                UcpdControlMessage<LL> message(SoftReset);
                UcpdParser<LL>::sendMessage(&message);
            }
        }
    }
}
;
template<class LL>
volatile uint32_t UcpdInternal<LL>::expectResponseTime = 0;
template<class LL>
volatile uint32_t UcpdInternal<LL>::resendTime = 0;
template<class LL>
volatile uint32_t UcpdInternal<LL>::hardResetAt = 0;
template<class LL>
volatile bool UcpdInternal<LL>::hardResetTiming = false;
template<class LL>
volatile bool UcpdInternal<LL>::resendRequestTiming = false;
template<class LL>
volatile bool UcpdInternal<LL>::expectResponse = false;

template<class LL>
bool UcpdInternal<LL>::useChunking = true;

template<class LL>
void (*UcpdInternal<LL>::powerStatusChangeCallback)(PowerStatus, PowerContract) = NULL;
template<class LL>
void (*UcpdInternal<LL>::messageOfInterestCallback)(UcpdMessage<LL>*) = &UcpdInternal<LL>::doNotUseMessage;
template<class LL>
uint32_t *UcpdInternal<LL>::unInitialisedStore = NULL;

template<class LL>
uint16_t UcpdInternal<LL>::absMinVoltage = 0;
template<class LL>
uint16_t UcpdInternal<LL>::absMaxVoltage = 0;
template<class LL>
VoltageNegotiationStrategy UcpdInternal<LL>::strategy = HighestPower;

template<class LL>
PowerContract UcpdInternal<LL>::currentContract(0, 0, 0);

template<class LL>
PowerContract UcpdInternal<LL>::requestedContract(0, 0, 0);

template<class LL>
uint16_t UcpdInternal<LL>::currentContractMaxCurrent = 0;
template<class LL>
uint8_t UcpdInternal<LL>::currentContractObjectPosition = 0;

template<class LL>
uint16_t UcpdInternal<LL>::targetAbsMinVoltage = 0;
template<class LL>
uint16_t UcpdInternal<LL>::targetAbsMaxVoltage = 0;

template<class LL>
uint16_t UcpdInternal<LL>::targetCurrent = 0;

template<class LL>
uint8_t UcpdInternal<LL>::hardResetCount = 0;
template<class LL>
bool UcpdInternal<LL>::hasAttemptedSoftReset = false;
template<class LL>
bool UcpdInternal<LL>::hasSentHardReset = false;

template<class LL>
bool UcpdInternal<LL>::voltageMismatch = false;

template<class LL>
bool UcpdInternal<LL>::hasContract = false;

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDINTERNAL_HPP_ */

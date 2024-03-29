/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDSTATUSMESSAGE_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDSTATUSMESSAGE_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "specific/UcpdInternal.hpp"

struct UcpdStatusPresentInput {
    bool externalPower :1;
    bool externalAc :1;
    bool internalBatteryPower :1;
    bool internalOtherPower :1;

    uint8_t getField() const {
        return (externalAc ? 0x1 : 0) | (externalPower ? 0x2 : 0) | (internalBatteryPower ? 0x4 : 0) | (internalOtherPower ? 0x8 : 0);
    }
};
struct UcpdStatusCurrentBatterySource {
    bool fixedBattery1 :1;
    bool fixedBattery2 :1;
    bool fixedBattery3 :1;
    bool fixedBattery4 :1;
    bool hotswappableBattery1 :1;
    bool hotswappableBattery2 :1;
    bool hotswappableBattery3 :1;
    bool hotswappableBattery4 :1;

    uint8_t getField() const {
        return (fixedBattery1 ? 0x1 : 0) | (fixedBattery2 ? 0x2 : 0) |
                (fixedBattery3 ? 0x4 : 0) | (fixedBattery4 ? 0x8 : 0) |
                (hotswappableBattery1 ? 0x10 : 0) | (hotswappableBattery2 ? 0x20 : 0) |
                (hotswappableBattery3 ? 0x40 : 0) | (hotswappableBattery4 ? 0x80 : 0);
    }
};

struct UcpdStatusEventFlags {
    bool OCP :1;
    bool OTP :1;
    bool OVP :1;
    bool CF :1; // CF/notCV

    uint8_t getField() const {
        return (OCP ? 0x1 : 0) | (OVP ? 0x2 : 0) |
                (OTP ? 0x4 : 0) | (CF ? 0x8 : 0);
    }
};
enum UcpdStatusTemperatureStatus {
    UcpdStatusTempNotSupported = 0,
    UcpdStatusTempNormal = 1,
    UcpdStatusTempWarning = 2,
    UcpdStatusTempOverTemperature = 3,
};
struct UcpdStatusPowerStatus {
    bool limitedByCable :1;
    bool limitedByOtherPorts :1;
    bool limitedByExternalPower :1;
    bool limitedByEvents :1;
    bool limitedByTemperatue :1;

    uint8_t getField() const {
        return (limitedByCable ? 0x1 : 0) | (limitedByOtherPorts ? 0x2 : 0) | (limitedByExternalPower ? 0x4 : 0) |
                (limitedByEvents ? 0x8 : 0) | (limitedByTemperatue ? 0x10 : 0);
    }
};
enum UcpdPowerState {
    UcpdPowerStateNotSupported = 0,
    UcpdPowerStateS0 = 1,
    UcpdPowerStateModernStandby = 2,
    UcpdPowerStateS3 = 3,
    UcpdPowerStateS4 = 4,
    UcpdPowerStateS5 = 5,
    UcpdPowerStateG3 = 6,
};
enum UcpdPowerStateIndicator {
    OffLED = 0,
    OnLED = 1,
    BlinkingLED = 2,
    BreathingLED = 3
};
struct UcpdStatusPowerStateChange {
    UcpdPowerState state :3;
    UcpdPowerStateIndicator indicator :3;
};

template<class LL>
class UcpdParsedStatusMessage {
    bool valid = false;
    uint8_t internalTemperature = 0;
    UcpdStatusPresentInput input;
    UcpdStatusCurrentBatterySource batterySource;
    UcpdStatusEventFlags eventFlags;
    UcpdStatusTemperatureStatus tempStatus = UcpdStatusTempNotSupported;

    UcpdStatusPowerStatus powerStatus;
    UcpdStatusPowerStateChange powerStateChange;

public:
    UcpdParsedStatusMessage() {
    }
    UcpdParsedStatusMessage(UcpdExtendedMessage<LL> *message) {
        valid = true;
        internalTemperature = message->getData()[0];

        input.externalAc = (message->getData()[1] & 0x02) != 0;
        input.externalPower = (message->getData()[1] & 0x04) != 0;
        input.internalBatteryPower = (message->getData()[1] & 0x08) != 0;
        input.internalOtherPower = (message->getData()[1] & 0x10) != 0;

        batterySource.fixedBattery1 = (message->getData()[2] & 0x01) != 0;
        batterySource.fixedBattery2 = (message->getData()[2] & 0x02) != 0;
        batterySource.fixedBattery3 = (message->getData()[2] & 0x04) != 0;
        batterySource.fixedBattery4 = (message->getData()[2] & 0x08) != 0;
        batterySource.hotswappableBattery1 = (message->getData()[2] & 0x10) != 0;
        batterySource.hotswappableBattery2 = (message->getData()[2] & 0x20) != 0;
        batterySource.hotswappableBattery3 = (message->getData()[2] & 0x40) != 0;
        batterySource.hotswappableBattery4 = (message->getData()[2] & 0x80) != 0;

        eventFlags.OCP = (message->getData()[3] & 0x02) != 0;
        eventFlags.OVP = (message->getData()[3] & 0x04) != 0;
        eventFlags.OTP = (message->getData()[3] & 0x08) != 0;
        eventFlags.CF = (message->getData()[3] & 0x10) != 0;

        tempStatus = (UcpdStatusTemperatureStatus) ((message->getData()[4] >> 1) & 0x03);

        powerStatus.limitedByCable = (message->getData()[5] & 0x02) != 0;
        powerStatus.limitedByOtherPorts = (message->getData()[5] & 0x04) != 0;
        powerStatus.limitedByExternalPower = (message->getData()[5] & 0x08) != 0;
        powerStatus.limitedByEvents = (message->getData()[5] & 0x10) != 0;
        powerStatus.limitedByTemperatue = (message->getData()[5] & 0x20) != 0;

        powerStateChange.state = (UcpdPowerState) (message->getData()[6] & 0x07);
        powerStateChange.indicator = (UcpdPowerStateIndicator) ((message->getData()[6] >> 3) & 0x07);
    }
    bool isValid() const {
        return valid;
    }
    uint8_t getInternalTemperature() const {
        return internalTemperature;
    }
    UcpdStatusPresentInput getPowerInput() const {
        return input;
    }
    UcpdStatusCurrentBatterySource getBatteryInput() const {
        return batterySource;
    }
    UcpdStatusEventFlags getEventFlags() const {
        return eventFlags;
    }
    UcpdStatusTemperatureStatus getTemperatureStatus() const {
        return tempStatus;
    }
    UcpdStatusPowerStatus getPowerStatus() const {
        return powerStatus;
    }
    UcpdStatusPowerStateChange getPowerStateChange() const {
        return powerStateChange;
    }
};

#endif /* SRC_MAIN_CPP_ARM_NO_OS_USBC_PD_MODULE_LOWLEVEL_UCPDPARSEDSTATUSMESSAGE_HPP_ */

/*
 * UcpdParsedExtendedSourceCapabilitiesMessage.hpp
 *
 *  Created on: 7 Apr 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDEXTENDEDSOURCECAPABILITIESMESSAGE_HPP_
#define SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDEXTENDEDSOURCECAPABILITIESMESSAGE_HPP_

#include <llIncludes.hpp>
#include "specific/UcpdInternal.hpp"

enum UcpdCapLoadStepRate { // per us
    mA150, mA500
};
enum UcpdCapLoadStepMagnitude { // percent
    IoC25, IoC90
};
enum UcpdCapTouchTemp {
    Default, TS1, TS2
};
struct UcpdCapVoltageRegulation {
    UcpdCapLoadStepRate loadStepRate :2;
    UcpdCapLoadStepMagnitude loadStepMagnitude :2;
};

struct UcpdCapCompliance {
    bool LPSCompliant :1;
    bool PS1Compliant :1;
    bool PS2Compliant :1;
    uint8_t getField() {
        return (LPSCompliant ? 0x1 : 0)
                | (PS1Compliant ? 0x2 : 0)
                | (PS2Compliant ? 0x4 : 0);
    }
};

struct UcpdCapTouchCurrent {
    bool lowTouchCurrentEPS :1;
    bool groundPinSupported :1;
    bool groundPinIntendedForEarth :1;

    uint8_t getField() {
        return (lowTouchCurrentEPS ? 0x1 : 0)
                | (groundPinSupported ? 0x2 : 0)
                | (groundPinIntendedForEarth ? 0x4 : 0);
    }
};

struct UcpdCapPeekCurrent {
    uint8_t overloadCurrent :5; // 10% increments, max:250%
    uint8_t overloadPeriod :6; // 20ms units
    uint8_t dutyCycle :4; // 5% increments
    bool vBusVoltageDroop :1;

    uint16_t getField() {
        uint16_t peak = 0;
        peak |= overloadCurrent;
        peak |= overloadPeriod << 5;
        peak |= dutyCycle << 11;
        peak |= vBusVoltageDroop ? 0x8000 : 0;
        return peak;
    }
};

struct UcpdCapSourceInputs {
    bool externalSupplyPresent :1;
    bool externalSupplyUnconstrained :1;
    bool internalBatteryPresent :1;
};

template<class LL>
class UcpdParsedExtendedSourceCapabilitiesMessage {
    bool valid = false;
    uint16_t VID = 0;
    uint16_t PID = 0;
    uint32_t XID = 0;

    uint8_t fwVersion = 0;
    uint8_t hwVersion = 0;

    UcpdCapVoltageRegulation regulation;

    uint8_t holdupTime = 3;

    UcpdCapCompliance compliance;
    UcpdCapTouchCurrent touchCurrent;

    UcpdCapPeekCurrent peekCurrent1;
    UcpdCapPeekCurrent peekCurrent2;
    UcpdCapPeekCurrent peekCurrent3;

    UcpdCapTouchTemp touchTemp = Default;
    UcpdCapSourceInputs sourceInputs;
    uint8_t numberOfFixedBatteries = 0;
    uint8_t numberOfHotswappableBatteries = 0;
    uint8_t sourcePdpRating = 0;

public:
    UcpdParsedExtendedSourceCapabilitiesMessage() {
    }
    UcpdParsedExtendedSourceCapabilitiesMessage(UcpdExtendedMessage<LL> *message) {
        valid = true;
        VID = message->getData()[1] << 8 | message->getData()[0];
        PID = message->getData()[3] << 8 | message->getData()[2];
        XID = message->getData()[7] << 24 | message->getData()[6] << 16 | message->getData()[5] << 8 | message->getData()[4];
        fwVersion = message->getData()[8];
        hwVersion = message->getData()[9];

        regulation.loadStepRate = (UcpdCapLoadStepRate) (message->getData()[10] & 0x3);
        regulation.loadStepMagnitude = (UcpdCapLoadStepMagnitude) ((message->getData()[10] >> 2) & 0x1);

        holdupTime = message->getData()[11];

        compliance.LPSCompliant = (message->getData()[12] & 0x01) != 0;
        compliance.PS1Compliant = (message->getData()[12] & 0x02) != 0;
        compliance.PS2Compliant = (message->getData()[12] & 0x04) != 0;

        touchCurrent.lowTouchCurrentEPS = (message->getData()[13] & 0x01) != 0;
        touchCurrent.groundPinSupported = (message->getData()[13] & 0x02) != 0;
        touchCurrent.groundPinIntendedForEarth = (message->getData()[13] & 0x04) != 0;

        peekCurrent1.overloadCurrent = (message->getData()[15] & 0x1F);
        peekCurrent1.overloadPeriod = (message->getData()[15] & 0xE0) >> 5 | (message->getData()[14] & 0x07) << 3;
        peekCurrent1.dutyCycle = (message->getData()[14] & 0x78) >> 3;
        peekCurrent1.vBusVoltageDroop = (message->getData()[14] & 0x80) != 0;

        peekCurrent2.overloadCurrent = (message->getData()[17] & 0x1F);
        peekCurrent2.overloadPeriod = (message->getData()[17] & 0xE0) >> 5 | (message->getData()[16] & 0x07) << 3;
        peekCurrent2.dutyCycle = (message->getData()[16] & 0x78) >> 3;
        peekCurrent2.vBusVoltageDroop = (message->getData()[16] & 0x80) != 0;

        peekCurrent3.overloadCurrent = (message->getData()[19] & 0x1F);
        peekCurrent3.overloadPeriod = (message->getData()[19] & 0xE0) >> 5 | (message->getData()[18] & 0x07) << 3;
        peekCurrent3.dutyCycle = (message->getData()[18] & 0x78) >> 3;
        peekCurrent3.vBusVoltageDroop = (message->getData()[18] & 0x80) != 0;

        touchTemp = (UcpdCapTouchTemp) (message->getData()[20]);

        sourceInputs.externalSupplyPresent = (message->getData()[21] & 0x01) != 0;
        sourceInputs.externalSupplyUnconstrained = (message->getData()[21] & 0x02) != 0;
        sourceInputs.internalBatteryPresent = (message->getData()[21] & 0x04) != 0;
        numberOfFixedBatteries = (message->getData()[22] & 0x0F);
        numberOfHotswappableBatteries = (message->getData()[22] & 0xF0) >> 4;

        sourcePdpRating = message->getData()[23];
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

    uint32_t getXID() const {
        return XID;
    }

    uint8_t getFwVersion() const {
        return fwVersion;
    }

    uint8_t getHwVersion() const {
        return hwVersion;
    }

    UcpdCapVoltageRegulation getVoltageRegulation() const {
        return regulation;
    }

    uint8_t getHoldupTime() const {
        return holdupTime;
    }

    UcpdCapCompliance getCompliance() const {
        return compliance;
    }

    UcpdCapTouchCurrent getTouchCurrent() const {
        return touchCurrent;
    }

    UcpdCapPeekCurrent getPeekCurrent1() const {
        return peekCurrent1;
    }
    UcpdCapPeekCurrent getPeekCurrent2() const {
        return peekCurrent2;
    }
    UcpdCapPeekCurrent getPeekCurrent3() const {
        return peekCurrent3;
    }

    UcpdCapTouchTemp getTouchTemp() const {
        return touchTemp;
    }

    UcpdCapSourceInputs getSourceInputs() const {
        return sourceInputs;
    }

    uint8_t getNumberOfFixedBatteries() const {
        return numberOfFixedBatteries;
    }

    uint8_t getNumberOfHotswappableBatteries() const {
        return numberOfHotswappableBatteries;
    }

    uint8_t getSourcePdpRating() const {
        return sourcePdpRating;
    }

};

#endif /* SRC_TEST_CPP_USB_PD_COMMANDS_UCPDPARSEDEXTENDEDSOURCECAPABILITIESMESSAGE_HPP_ */

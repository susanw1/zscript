/*
 * UcpdMessages.hpp
 *
 *  Created on: 17 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_USB_PD_UCPDMESSAGES_HPP_
#define SRC_TEST_CPP_USB_PD_UCPDMESSAGES_HPP_
#include <llIncludes.hpp>

template<class LL>
int findByteInvertedAddress(int i);
template<class LL>
int findDataObjectInvertedAddress(int i);
template<class LL>
int findBytePairInvertedAddress(int i);

enum UcpdMessageType {
    ControlMessage, DataMessage, ExtendedMessage
};

enum UcpdControlMessageType {
    UcpdControlReserved0,
    GoodCRC,
    GotoMin,
    Accept,
    Reject,
    Ping,
    PSRdy,
    GetSourceCap,
    GetSinkCap,
    DRSwap,
    PRSwap,
    VCONNSwap,
    Wait,
    SoftReset,
    DataReset,
    DataResetComplete,
    NotSupported,
    GetSourceCapExtended,
    GetStatus,
    FRSwap,
    GetPPSStatus,
    GetCountryCodes,
    GetSinkCapExtended
};
enum UcpdDataMessageType {
    UcpdDataReserved0,
    SourceCapabilities,
    Request,
    BIST,
    SinkCapabilities,
    BatteryStatus,
    Alert,
    GetCountryInfo,
    EnterUSB,
    UcpdReserved9,
    UcpdReserved10,
    UcpdReserved11,
    UcpdReserved12,
    UcpdReserved13,
    UcpdReserved14,
    VenderDefined
};
enum UcpdExtendedMessageType {
    UcpdExtendedReserved0,
    SourceCapabilitiesExtended,
    Status,
    GetBatteryCap,
    GetBatteryStatus,
    BatteryCapabilities,
    GetManufacturerInfo,
    ManufacturerInfo,
    SecurityRequest,
    SecurityResponse,
    FirmwareUpdateRequest,
    FirmwareUpdateResponse,
    PPSStatus,
    CountryInfo,
    CountryCodes,
    SinkCapabilitiesExtended
};

template<class LL>
class UcpdMessage {
    UcpdMessageType type;

protected:
    UcpdMessage(UcpdMessageType type) :
            type(type) {

    }
public:
    UcpdMessageType getUcpdMessageType() {
        return type;
    }
    virtual void encode(uint8_t *target, uint8_t messageId) = 0; //assumes is given a long enough byte array - e.g. txBuffer
    virtual uint16_t getLength() = 0;
    virtual ~UcpdMessage() {
    }
};

template<class LL>
class UcpdControlMessage: public UcpdMessage<LL> {
    UcpdControlMessageType controlType;

public:
    UcpdControlMessage(UcpdControlMessageType controlType) :
            UcpdMessage<LL>(ControlMessage), controlType(controlType) {
    }
    UcpdControlMessageType getUcpdControlMessageType() {
        return controlType;
    }
    virtual void encode(uint8_t *target, uint8_t messageId) {
        target[findByteInvertedAddress<LL>(0)] = messageId << 1; //always assumes is Sink
        target[findByteInvertedAddress<LL>(1)] = 0x80; //always assumes is UFP
        target[findByteInvertedAddress<LL>(1)] |= (uint8_t) controlType;
    }
    virtual uint16_t getLength() {
        return 2;
    }
};

template<class LL>
class UcpdDataMessage: public UcpdMessage<LL> {
protected:
    UcpdDataMessageType dataType;

public:
    UcpdDataMessage(UcpdDataMessageType dataType) :
            UcpdMessage<LL>(DataMessage), dataType(dataType) {
    }

    UcpdDataMessageType getUcpdDataMessageType() {
        return dataType;
    }

    virtual void encode(uint8_t *target, uint8_t messageId) { //placeholder for messages which are never sent, only received
        (void) target;
        (void) messageId;
    }
    virtual uint16_t getLength() {
        return 0;
    }
};
template<class LL>
class PowerDataObject {
    uint16_t maximumVoltage;
    uint16_t minimumVoltage;
    uint16_t maximumCurrent;

public:
    PowerDataObject() :
            maximumVoltage(0), minimumVoltage(0), maximumCurrent(0) {
    }
    PowerDataObject(uint16_t maximumVoltage, uint16_t minimumVoltage, uint16_t maximumCurrent) :
            maximumVoltage(maximumVoltage), minimumVoltage(minimumVoltage), maximumCurrent(maximumCurrent) {
    }
    uint16_t getMaximumVoltage() const { //50mV units
        return maximumVoltage;
    }
    uint16_t getMinimumVoltage() const { //50mV units
        return minimumVoltage;
    }
    uint16_t getMaximumCurrent() const { //10mA units
        return maximumCurrent;
    }
    void encode(uint8_t *target) const {
        if (minimumVoltage == maximumVoltage) {
            uint32_t pdo = (maximumCurrent & 0x3FF) | ((minimumVoltage & 0x3FF) << 10);
            target[findDataObjectInvertedAddress<LL>(0)] = (pdo >> 24) & 0xFF;
            target[findDataObjectInvertedAddress<LL>(1)] = (pdo >> 16) & 0xFF;
            target[findDataObjectInvertedAddress<LL>(2)] = (pdo >> 8) & 0xFF;
            target[findDataObjectInvertedAddress<LL>(3)] = pdo & 0xFF;
        } else {
            uint32_t pdo = (maximumCurrent & 0x3FF) | ((minimumVoltage & 0x3FF) << 10) | ((maximumVoltage & 0x3FF) << 20) | (0x2 << 30);
            target[findDataObjectInvertedAddress<LL>(0)] = (pdo >> 24) & 0xFF;
            target[findDataObjectInvertedAddress<LL>(1)] = (pdo >> 16) & 0xFF;
            target[findDataObjectInvertedAddress<LL>(2)] = (pdo >> 8) & 0xFF;
            target[findDataObjectInvertedAddress<LL>(3)] = pdo & 0xFF;
        }
    }
};

template<class LL>
class UcpdSourceCapabilitiesMessage: public UcpdDataMessage<LL> {
    PowerDataObject<LL> pdos[7];
    uint8_t pdoCount = 0;
    bool usbSuspendRequired;
    bool unconstrainedPower;
    bool usbCommunicationsCapable;
    bool dualRoleData;
    bool unchunkedExtendedMessagesSupported;

public:
    UcpdSourceCapabilitiesMessage() :
            UcpdDataMessage<LL>(SourceCapabilities), usbSuspendRequired(false), unconstrainedPower(false), usbCommunicationsCapable(
                    false), dualRoleData(false), unchunkedExtendedMessagesSupported(false) {
    }
    UcpdSourceCapabilitiesMessage(bool usbSuspendRequired, bool unconstrainedPower, bool usbCommunicationsCapable, bool dualRoleData,
            bool unchunkedExtendedMessagesSupported) :
            UcpdDataMessage<LL>(SourceCapabilities), usbSuspendRequired(usbSuspendRequired), unconstrainedPower(unconstrainedPower), usbCommunicationsCapable(
                    usbCommunicationsCapable), dualRoleData(dualRoleData), unchunkedExtendedMessagesSupported(unchunkedExtendedMessagesSupported) {
    }
    void addPDO(PowerDataObject<LL> obj) {
        pdos[pdoCount++] = obj;
    }

    bool isUsbSuspendRequired() const {
        return usbSuspendRequired;
    }

    bool hasUnconstrainedPower() const {
        return unconstrainedPower;
    }

    bool isUsbCommunicationsCapable() const {
        return usbCommunicationsCapable;
    }

    bool hasDualRoleData() const {
        return dualRoleData;
    }

    bool hasUnchunkedExtendedMessageSupport() const {
        return unchunkedExtendedMessagesSupported;
    }

    const PowerDataObject<LL>* getPdos() const {
        return pdos;
    }
    uint8_t getPdoNum() const {
        return pdoCount;
    }
};

template<class LL>
class UcpdSinkCapabilitiesMessage: public UcpdDataMessage<LL> {
    PowerDataObject<LL> pdos[7];
    uint8_t pdoCount = 0;
    bool dualRolePower;
    bool higherCapability;
    bool unconstrainedPower;
    bool usbCommunicationsCapable;
    bool dualRoleData;

public:
    UcpdSinkCapabilitiesMessage(bool dualRolePower, bool higherCapability, bool unconstrainedPower, bool usbCommunicationsCapable, bool dualRoleData) :
            UcpdDataMessage<LL>(SinkCapabilities), dualRolePower(dualRolePower), higherCapability(higherCapability),
                    unconstrainedPower(unconstrainedPower), usbCommunicationsCapable(usbCommunicationsCapable), dualRoleData(dualRoleData) {
    }
    void addPDO(PowerDataObject<LL> obj) {
        pdos[pdoCount++] = obj;
    }
    bool hasDualRolePower() {
        return dualRoleData;
    }
    bool hasHigherCapability() {
        return higherCapability;
    }

    bool hasUnconstrainedPower() {
        return unconstrainedPower;
    }

    bool isUsbCommunicationsCapable() {
        return usbCommunicationsCapable;
    }

    bool hasDualRoleData() {
        return dualRoleData;
    }

    const PowerDataObject<LL>* getPdos() {
        return pdos;
    }
    uint8_t getPdoNum() {
        return pdoCount;
    }

    virtual void encode(uint8_t *target, uint8_t messageId) {
        target[findByteInvertedAddress<LL>(0)] = (messageId << 1) | (pdoCount << 4); //always assumes is Sink
        target[findByteInvertedAddress<LL>(1)] = 0x80; //always assumes is UFP
        target[findByteInvertedAddress<LL>(1)] |= (uint8_t) this->dataType;
        for (int i = 0; i < pdoCount; ++i) {
            pdos[i].encode(target + 4 * i + 2);
        }
        if (dualRolePower) {
            target[findByteInvertedAddress<LL>(2)] |= 0x20;
        }
        if (higherCapability) {
            target[findByteInvertedAddress<LL>(2)] |= 0x10;
        }
        if (unconstrainedPower) {
            target[findByteInvertedAddress<LL>(2)] |= 0x08;
        }
        if (usbCommunicationsCapable) {
            target[findByteInvertedAddress<LL>(2)] |= 0x04;
        }
        if (dualRoleData) {
            target[findByteInvertedAddress<LL>(2)] |= 0x02;
        }
        // presuming FRS not supported, and that first PDO is vSafe5V like it should be
    }
    virtual uint16_t getLength() {
        return 2 + 4 * pdoCount;
    }
};

template<class LL>
class UcpdRequestMessage: public UcpdDataMessage<LL> {
    uint8_t objectPosition;
    bool giveBack;
    bool capabilityMismatch;
    bool usbCommunicationsCapable;
    bool ignoreUsbSuspend;
    bool unchunkedExtendedMessagesSupported;
    uint16_t operatingCurrentDraw; //10mA units
    uint16_t maximumCurrentDraw; //10mA units

public:
    UcpdRequestMessage(uint8_t objectPosition, bool giveBack, bool capabilityMismatch, bool usbCommunicationsCapable, bool ignoreUsbSuspend,
            bool unchunkedExtendedMessagesSupported, uint16_t operatingCurrentDraw, uint16_t maximumCurrentDraw) :
            UcpdDataMessage<LL>(Request), objectPosition(objectPosition), giveBack(giveBack), capabilityMismatch(capabilityMismatch),
                    usbCommunicationsCapable(usbCommunicationsCapable), ignoreUsbSuspend(ignoreUsbSuspend),
                    unchunkedExtendedMessagesSupported(unchunkedExtendedMessagesSupported), operatingCurrentDraw(operatingCurrentDraw), maximumCurrentDraw(
                            maximumCurrentDraw) {
    }
    uint8_t getObjectPosition() {
        return objectPosition;
    }
    bool hasGiveBack() {
        return giveBack;
    }
    bool hasCapabilityMismatch() {
        return capabilityMismatch;
    }

    bool canIgnoreUsbSuspend() {
        return ignoreUsbSuspend;
    }

    bool isUsbCommunicationsCapable() {
        return usbCommunicationsCapable;
    }

    bool hasUnchunkedExtendedMessageSupport() {
        return unchunkedExtendedMessagesSupported;
    }
    uint16_t getOperatingCurrentDraw() {
        return operatingCurrentDraw;
    }
    uint16_t getMaximumCurrentDraw() {
        return maximumCurrentDraw;
    }
    virtual void encode(uint8_t *target, uint8_t messageId) {
        target[findByteInvertedAddress<LL>(0)] = (messageId << 1) | (1 << 4); //always assumes is Sink
        target[findByteInvertedAddress<LL>(1)] = 0x80; //always assumes is UFP
        target[findByteInvertedAddress<LL>(1)] |= (uint8_t) this->dataType;
        uint32_t rdo = (maximumCurrentDraw & 0x3FF) | ((operatingCurrentDraw & 0x3FF) << 10);
        rdo |= (objectPosition + 1) << 28;
        if (giveBack) {
            rdo |= 0x08000000;
        }
        if (capabilityMismatch) {
            rdo |= 0x04000000;
        }
        if (usbCommunicationsCapable) {
            rdo |= 0x02000000;
        }
        if (ignoreUsbSuspend) {
            rdo |= 0x01000000;
        }
        if (unchunkedExtendedMessagesSupported) {
            rdo |= 0x00800000;
        }
        target[findByteInvertedAddress<LL>(2)] = (rdo >> 24) & 0xFF;
        target[findByteInvertedAddress<LL>(3)] = (rdo >> 16) & 0xFF;
        target[findByteInvertedAddress<LL>(4)] = (rdo >> 8) & 0xFF;
        target[findByteInvertedAddress<LL>(5)] = rdo & 0xFF;
    }
    virtual uint16_t getLength() {
        return 6;
    }
};

template<class LL>
class UcpdAlertMessage: public UcpdDataMessage<LL> {
    bool OcpEvent;
    bool OtpEvent;
    bool OvpEvent;

public:
    UcpdAlertMessage(bool OcpEvent, bool OtpEvent, bool OvpEvent) :
            UcpdDataMessage<LL>(Alert), OcpEvent(OcpEvent), OtpEvent(OtpEvent), OvpEvent(OvpEvent) {
    }
    bool hasOcpEvent() {
        return OcpEvent;
    }
    bool hasOtpEvent() {
        return OtpEvent;
    }

    bool hasOvpEvent() {
        return OvpEvent;
    }
};

enum UcpdUsbMode {
    Usb2Mode, Usb3Mode, Usb4Mode
};
enum UcpdCableSpeed {
    Usb2, Usb3Gen1, Usb3Gen2, Usb4
};
enum UcpdCableType {
    Passive, ActiveReTimer, ActiveReDriver, OpticallyIsolated
};
enum UcpdCableCurrent {
    NoVBus, Reserved1, Current3A, Current5A
};

template<class LL>
class UcpdEnterUsbMessage: public UcpdDataMessage<LL> {
    UcpdUsbMode mode;
    bool Usb4Device;
    bool Usb3Device;
    UcpdCableSpeed cableSpeed;
    UcpdCableType cableType;
    UcpdCableCurrent cableCurrent;
    bool PCIeTunnel;
    bool DPTunnel;
    bool TBTSupport;
    bool HostPresent;

public:
    UcpdEnterUsbMessage(UcpdUsbMode mode, bool Usb4Device, bool Usb3Device, UcpdCableSpeed cableSpeed, UcpdCableType cableType, UcpdCableCurrent cableCurrent,
            bool PCIeTunnel, bool DPTunnel, bool TBTSupport, bool HostPresent) :
            UcpdDataMessage<LL>(EnterUSB), mode(mode), Usb4Device(Usb4Device), Usb3Device(Usb3Device), cableSpeed(cableSpeed), cableType(cableType),
                    cableCurrent(cableCurrent), PCIeTunnel(PCIeTunnel), DPTunnel(DPTunnel), TBTSupport(TBTSupport), HostPresent(HostPresent) {
    }
    UcpdUsbMode getUsbMode() {
        return mode;
    }
    UcpdCableSpeed getCableSpeed() {
        return cableSpeed;
    }
    UcpdCableType getCableType() {
        return cableType;
    }
    UcpdCableCurrent getCableCurrent() {
        return cableCurrent;
    }
    bool supportsUsb4Device() {
        return Usb4Device;
    }
    bool supportsUsb3Device() {
        return Usb3Device;
    }
    bool canPCIeTunnel() {
        return PCIeTunnel;
    }
    bool canDPTunnel() {
        return DPTunnel;
    }
    bool hasTBTSupport() {
        return TBTSupport;
    }
    bool isHostPresent() {
        return HostPresent;
    }
};

template<class LL>
class UcpdExtendedMessage: public UcpdMessage<LL> {
    UcpdExtendedMessageType extendedType;
    bool chunked;
    uint8_t chunkNum;
    bool chunkRequest;
    uint16_t dataSize;
    const uint8_t *data;

public:
    UcpdExtendedMessage(UcpdExtendedMessageType extendedType, bool chunked, uint8_t chunkNum, bool chunkRequest, uint16_t dataSize, const uint8_t *data) :
            UcpdMessage<LL>(ExtendedMessage), extendedType(extendedType), chunked(chunked), chunkNum(chunkNum), chunkRequest(chunkRequest), dataSize(dataSize),
                    data(data) {
    }

    UcpdExtendedMessageType getUcpdExtendedMessageType() {
        return extendedType;
    }
    bool isChunked() {
        return chunked;
    }
    bool isChunkRequest() {
        return chunkRequest;
    }
    uint8_t getChunkNum() {
        return chunkNum;
    }
    uint16_t getDataSize() {
        return dataSize;
    }
    const uint8_t* getData() const {
        return data;
    }
    virtual void encode(uint8_t *target, uint8_t messageId) {
        if (chunked) {
            target[findByteInvertedAddress<LL>(0)] = 0x80 | (messageId << 1) | (((dataSize + 5) / 4) << 4);
        } else {
            target[findByteInvertedAddress<LL>(0)] = 0x80 | (messageId << 1);
        }
        target[findByteInvertedAddress<LL>(1)] = 0x80; //always assumes is UFP
        target[findByteInvertedAddress<LL>(1)] |= (uint8_t) extendedType;
        target[findBytePairInvertedAddress<LL>(0) + 2] = 0;
        if (chunked) {
            target[findBytePairInvertedAddress<LL>(0) + 2] |= 0x80 | (chunkNum << 3);
        }
        if (chunkRequest) {
            target[findBytePairInvertedAddress<LL>(0) + 2] |= 0x04;
        }
        target[findBytePairInvertedAddress<LL>(0) + 2] |= (dataSize >> 8) & 1;
        target[findBytePairInvertedAddress<LL>(1) + 2] = dataSize & 0xFF;
        for (int i = 0; i < dataSize; ++i) {
            target[4 + i] = data[i];
        }
    }
    virtual uint16_t getLength() {
        return 4 + dataSize;
    }
};

template<class LL>
int findByteInvertedAddress(int i) {
    if (i < 2) {
        return 1 - i;
    }
    return ((i - 2) / 4) * 8 + 7 - i;
}
template<class LL>
int findDataObjectInvertedAddress(int i) {
    return 3 - i;
}

template<class LL>
int findBytePairInvertedAddress(int i) {
    return 1 - i;
}

#endif /* SRC_TEST_CPP_USB_PD_UCPDMESSAGES_HPP_ */

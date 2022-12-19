/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDPARSER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDPARSER_HPP_

#include <llIncludes.hpp>
#include "../../../../../../../../clock-ll/src/main/c++/clock-ll/ClockManager.hpp"
#include "../../../../../../../../clock-ll/src/main/c++/clock-ll/SystemMilliClock.hpp"
#include "UcpdLowLevel.hpp"
#include "UcpdMessages.hpp"

enum UcpdMessageSentStatus {
    GoodCrcReceived, RetriesFailed, WrongCrc, SoftResetFailed
};
template<class LL>
class UcpdParser {
    static void (*messageReceivedCallback)(UcpdMessage<LL>*);
    static void (*messageSentCallback)(UcpdMessageSentStatus);
    static uint32_t sendTimeMs;
    static uint16_t sendLength;
    static uint8_t txMessageId;
    static uint8_t prevRxMessageId;
    static uint8_t retryCounter;
    static uint8_t goodCrcMsg[2];
    static uint8_t softRstMsg[2];
    static bool isSendingSoftReset;
    static bool isSending;
    static bool hasBuffered;
    static bool isReceiving;
    static bool needsCrc;

    static void incMessageId() {
        txMessageId++;
        if (txMessageId > 7) {
            txMessageId = 0;
        }
    }
    static void setBufferMessageId(uint8_t *buffer, uint8_t id) {
        buffer[findByteInvertedAddress<LL>(0)] &= ~0x0E;
        buffer[findByteInvertedAddress<LL>(0)] |= (id & 0x7) << 1;
    }

    static void rxCallback(UcpdRxReceiveStatus status) {
        if (status == UcpdRxMessageReceived) {
            isReceiving = true;
            parseMessage();
        }
    }
    static void txCallback(UcpdTxTerminationStatus status) {
        if (status == UcpdTxMessageDiscarded) {
            isReceiving = true;
            isSending = false;
        } else if (status == UcpdMessageSent) {
            if (isReceiving && hasBuffered) {
                sendInternal();
            } else {
                hasBuffered = false;
                isSending = false;
            }
            isReceiving = false;
        } else {
            hasBuffered = false;
            isSending = false;
        }
    }
    static void goodCrcSentCallback(UcpdTxTerminationStatus status) {
        (void) status;
        if (isReceiving && hasBuffered) {
            sendInternal();
        }
        isReceiving = false;
    }

    static void sendGoodCRC();
    static void parseControlMessage(const uint8_t *rxBuf, uint16_t rxLen);
    static void parseDataMessage(const uint8_t *rxBuf, uint16_t rxLen);
    static void parseExtendedMessage(const uint8_t *rxBuf, uint16_t rxLen);
    static void parseMessage();

    static void sendInternal() {
        retryCounter = 0;
        isSendingSoftReset = false;
        sendTimeMs = SystemMilliClock<LL>::getTimeMillis();
        needsCrc = true;
        incMessageId();
        setBufferMessageId(UcpdLowLevel<LL>::getTxBuffer(), txMessageId);
        UcpdLowLevel<LL>::sendTxBuffer(sendLength, txCallback);
    }
public:
    static void init() {
        UcpdLowLevel<LL>::init();
        UcpdLowLevel<LL>::setRxCallback(rxCallback);
    }

    static void setHardResetCallback(void (*hardResetCallback)()) {
        UcpdLowLevel<LL>::setHardResetCallback(hardResetCallback);
    }
    static void setMessageReceivedCallback(void (*messageReceivedCallback)(UcpdMessage<LL>*)) {
        UcpdParser::messageReceivedCallback = messageReceivedCallback;
    }

    static void setMessageSentCallback(void (*messageSentCallback)(UcpdMessageSentStatus)) {
        UcpdParser::messageSentCallback = messageSentCallback;
    }
    static void sendHardReset() {
        isSending = false;
        retryCounter = 0;
        UcpdLowLevel<LL>::hardReset();
    }
    static void softReset() {
        retryCounter = 0;
        txMessageId = 0;
        prevRxMessageId = 8;
    }
    static void sendSoftReset();

    static void resendCurrent() {
        if (!isReceiving) {
            sendInternal();
        } else {
            hasBuffered = true;
        }
    }

    static bool sendMessage(UcpdMessage<LL> *message) {
        retryCounter = 0;
        if (isSending) {
            return false;
        }
        sendLength = message->getLength();
        message->encode(UcpdLowLevel<LL>::getTxBuffer(), txMessageId & 0x7);
        if (!isReceiving) {
            sendInternal();
        } else {
            hasBuffered = true;
        }
        isSending = true;
        return true;
    }

    static void tick() {
        if (needsCrc && SystemMilliClock<LL>::getTimeMillis() > sendTimeMs + 5) {
            if (retryCounter < 2) {
                retryCounter++;
                sendTimeMs = SystemMilliClock<LL>::getTimeMillis();
                needsCrc = true;
                if (isSendingSoftReset) {
                    UcpdLowLevel<LL>::sendBuffer(softRstMsg, 2, txCallback);
                } else {
                    UcpdLowLevel<LL>::sendTxBuffer(sendLength, txCallback);
                }
            } else {
                retryCounter = 0;
                needsCrc = false;
                if (isSendingSoftReset) {
                    messageSentCallback(SoftResetFailed);
                } else {
                    messageSentCallback(RetriesFailed);
                }
            }
        }
    }
};

template<class LL>
uint8_t UcpdParser<LL>::goodCrcMsg[2] = { 0x81, 0x00 };
template<class LL>
uint8_t UcpdParser<LL>::softRstMsg[2] = { 0x8D, 0x00 };

template<class LL>
void (*UcpdParser<LL>::messageReceivedCallback)(UcpdMessage<LL>*)= NULL;
template<class LL>
void (*UcpdParser<LL>::messageSentCallback)(UcpdMessageSentStatus) = NULL;
template<class LL>
uint32_t UcpdParser<LL>::sendTimeMs = 0;
template<class LL>
uint16_t UcpdParser<LL>::sendLength = 0;
template<class LL>
uint8_t UcpdParser<LL>::txMessageId = 0;
template<class LL>
uint8_t UcpdParser<LL>::prevRxMessageId = 8;
template<class LL>
uint8_t UcpdParser<LL>::retryCounter = 0;
template<class LL>
bool UcpdParser<LL>::isSending = false;
template<class LL>
bool UcpdParser<LL>::isSendingSoftReset = false;
template<class LL>
bool UcpdParser<LL>::hasBuffered = false;
template<class LL>
bool UcpdParser<LL>::isReceiving = false;
template<class LL>
bool UcpdParser<LL>::needsCrc = false;

template<class LL>
void UcpdParser<LL>::sendSoftReset() {
    retryCounter = 0;
    incMessageId();
    setBufferMessageId(softRstMsg, txMessageId);
    isSendingSoftReset = true;
    sendTimeMs = SystemMilliClock<LL>::getTimeMillis();
    needsCrc = true;
    UcpdLowLevel<LL>::sendBuffer(softRstMsg, 2, txCallback);
}
template<class LL>
void UcpdParser<LL>::sendGoodCRC() {
    setBufferMessageId(goodCrcMsg, prevRxMessageId);
    UcpdLowLevel<LL>::sendBuffer(goodCrcMsg, 2, goodCrcSentCallback);
}

template<class LL>
void UcpdParser<LL>::parseControlMessage(const uint8_t *rxBuf, uint16_t rxLen) {
    (void) rxLen; //FIXME: We kinda just assume we got the right length...
    uint8_t tmpMessageId = (rxBuf[findByteInvertedAddress<LL>(0)] >> 1) & 0x07;
    if ((rxBuf[findByteInvertedAddress<LL>(1)] & 0x1F) == GoodCRC) {
        if (isSendingSoftReset && txMessageId == tmpMessageId) {
            retryCounter = 0;
            messageSentCallback(GoodCrcReceived);
        } else if (txMessageId == tmpMessageId) {
            retryCounter = 0;
            needsCrc = false;
            messageSentCallback(GoodCrcReceived);
        } else {
            messageSentCallback(WrongCrc);
        }
        return;
    }
    if (tmpMessageId == prevRxMessageId) {
        sendGoodCRC();
        return;
    }
    prevRxMessageId = tmpMessageId;
    sendGoodCRC();
    if ((rxBuf[findByteInvertedAddress<LL>(1)] & 0x1F) == SoftReset) {
        retryCounter = 0;
        prevRxMessageId = 8;
    }

    UcpdControlMessage<LL> message((UcpdControlMessageType) (rxBuf[findByteInvertedAddress<LL>(1)] & 0x1F));
    messageReceivedCallback(&message);
}

template<class LL>
void UcpdParser<LL>::parseDataMessage(const uint8_t *rxBuf, uint16_t rxLen) {
    (void) rxLen; //FIXME: We kinda just assume we got the right length...
    uint8_t tmpMessageId = (rxBuf[findByteInvertedAddress<LL>(0)] >> 1) & 0x07;

    if (tmpMessageId == prevRxMessageId) {
        sendGoodCRC();
        return;
    }
    prevRxMessageId = tmpMessageId;
    sendGoodCRC();

    UcpdDataMessageType type = (UcpdDataMessageType) (rxBuf[findByteInvertedAddress<LL>(1)] & 0x1F);

    uint8_t dataObjectNum = (rxBuf[findByteInvertedAddress<LL>(0)] >> 4) & 0x7;

    switch (type) {
    case SourceCapabilities:
        {
        UcpdSourceCapabilitiesMessage<LL> capMessage(rxBuf[findByteInvertedAddress<LL>(2)] & 0x10, rxBuf[findByteInvertedAddress<LL>(2)] & 0x08,
                rxBuf[findByteInvertedAddress<LL>(2)] & 0x04, rxBuf[findByteInvertedAddress<LL>(2)] & 0x02,
                rxBuf[findByteInvertedAddress<LL>(2)] & 0x01);
        for (int i = 0; i < dataObjectNum; ++i) {
            uint32_t pdoRaw = (rxBuf[findByteInvertedAddress<LL>(2 + 4 * i)] << 24) | (rxBuf[findByteInvertedAddress<LL>(3 + 4 * i)] << 16)
                    | (rxBuf[findByteInvertedAddress<LL>(4 + 4 * i)] << 8) | rxBuf[findByteInvertedAddress<LL>(5 + 4 * i)];
            uint16_t maxCurrent = pdoRaw & 0x3FF;
            uint16_t minVoltage = (pdoRaw >> 10) & 0x3FF;
            uint16_t maxVoltage = minVoltage;
            if ((pdoRaw & 0xC0000000) == 0) {
                maxVoltage = minVoltage;
            } else if ((rxBuf[findByteInvertedAddress<LL>(2 + 4 * i)] & 0xC0) == 0x80) {
                maxVoltage = (pdoRaw >> 20) & 0x3FF;
            }
            PowerDataObject<LL> obj(maxVoltage, minVoltage, maxCurrent);
            capMessage.addPDO(obj);
        }
        messageReceivedCallback(&capMessage);
    }
        break;
    case Alert:
        {
        UcpdAlertMessage<LL> alertMessage(rxBuf[findByteInvertedAddress<LL>(2)] & 0x04, rxBuf[findByteInvertedAddress<LL>(2)] & 0x08,
                rxBuf[findByteInvertedAddress<LL>(2)] & 0x40);
        messageReceivedCallback(&alertMessage);
    }
        break;
    case EnterUSB:
        {
        UcpdEnterUsbMessage<LL> eUsbMessage((UcpdUsbMode) ((rxBuf[findByteInvertedAddress<LL>(2)] >> 4) & 0x7), rxBuf[findByteInvertedAddress<LL>(2)] & 0x4,
                rxBuf[findByteInvertedAddress<LL>(2)] & 0x2, (UcpdCableSpeed) ((rxBuf[findByteInvertedAddress<LL>(3)] >> 5) & 0x3),
                (UcpdCableType) ((rxBuf[findByteInvertedAddress<LL>(3)] >> 3) & 0x3), (UcpdCableCurrent) ((rxBuf[findByteInvertedAddress<LL>(3)] >> 1) & 0x3),
                rxBuf[findByteInvertedAddress<LL>(3)] & 0x1, rxBuf[findByteInvertedAddress<LL>(4)] & 0x80, rxBuf[findByteInvertedAddress<LL>(4)] & 0x40,
                rxBuf[findByteInvertedAddress<LL>(4)] & 0x20);
        messageReceivedCallback(&eUsbMessage);
    }
        break;
    default:
        {
        UcpdDataMessage<LL> message(type);
        messageReceivedCallback(&message);
    }
        break;
    }
}

template<class LL>
void UcpdParser<LL>::parseExtendedMessage(const uint8_t *rxBuf, uint16_t rxLen) {
    (void) rxLen; //FIXME: We kinda just assume we got the right length...
    uint8_t tmpMessageId = (rxBuf[findByteInvertedAddress<LL>(0)] >> 1) & 0x07;

    if (tmpMessageId == prevRxMessageId) {
        sendGoodCRC();
        return;
    }
    prevRxMessageId = tmpMessageId;
    sendGoodCRC();

    UcpdExtendedMessageType type = (UcpdExtendedMessageType) (rxBuf[findByteInvertedAddress<LL>(1)] & 0x1F);
    UcpdExtendedMessage<LL> message(type, rxBuf[findByteInvertedAddress<LL>(3)] & 0x80, (rxBuf[findByteInvertedAddress<LL>(3)] >> 3) & 0xF,
            rxBuf[findByteInvertedAddress<LL>(3)] & 0x4, rxBuf[findByteInvertedAddress<LL>(4)] & 0x1FF, NULL);
    messageReceivedCallback(&message);
}

template<class LL>
void UcpdParser<LL>::parseMessage() {
    const uint8_t *rxBuf = UcpdLowLevel<LL>::getRxBuffer();
    uint16_t rxLen = UcpdLowLevel<LL>::getRxLength();
    if ((rxBuf[findByteInvertedAddress<LL>(0)] & 0x80) != 0) {
        parseExtendedMessage(rxBuf, rxLen);
    } else if ((rxBuf[findByteInvertedAddress<LL>(0)] & 0x70) != 0) {
        parseDataMessage(rxBuf, rxLen);
    } else {
        parseControlMessage(rxBuf, rxLen);
    }
}

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USBC_PD_MODULE_LOWLEVEL_SPECIFIC_UCPDPARSER_HPP_ */

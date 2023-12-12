/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USB_MODULE_LOWLEVEL_SPECIFIC_USBDEFAULTENDPOINT_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USB_MODULE_LOWLEVEL_SPECIFIC_USBDEFAULTENDPOINT_HPP_

#include <llIncludes.hpp>
#include "UsbRegisters.hpp"
#include "UsbEndpointRegister.hpp"

enum UsbStandardControlMessageType {
    GET_STATUS = 0, // give 0 every time
    CLEAR_FEATURE = 1, // stall
    SET_FEATURE = 3, // stall
    SET_ADDRESS = 5, // set address
    GET_DESCRIPTOR = 6, // send descriptor
    SET_DESCRIPTOR = 7, // stall
    GET_CONFIGURATION = 8, // send configured ? 1 : 0
    SET_CONFIGURATION = 9, // set configured
    GET_INTERFACE = 10, // stall
    SET_INTERFACE = 11, // stall
    SYNCH_FRAME = 12, // stall
};

enum UsbClassControlMessageType {
    SEND_ENCAPSULATED_COMMAND = 0, // ignore
    GET_ENCAPSULATED_RESPONSE = 1, // return so many 0s
    SET_LINE_CODING = 0x20, // set lineCoding
    GET_LINE_CODING = 0x21, // send lineCoding
    SET_CONTROL_LINE_STATE = 0x22 // nothing
};

template<class LL>
class UsbDefaultEndpoint {
    static const uint16_t maxPacketLength = 64;
    static constexpr uint8_t deviceDescriptor[18] = { 18, 0x01, 0x00, 0x02, 0x02, 0x00, 1, 64, 0x00, 0x00, 0x00, 0x00, 0x10, 0, 0, 0, 0, 1 };
    static constexpr uint8_t configurationDescriptor[9] = { 9, 0x02, 67, 0, 2, 1, 0, 0x80, 0 };
    static constexpr uint8_t interface0Descriptor[9] = { 9, 0x04, 0, 0, 1, 0x02, 0x02, 1, 0 };
    static constexpr uint8_t interface0HeaderDescriptor[5] = { 5, 0x24, 0x00, 0x20, 0x01 };
    static constexpr uint8_t interface0CallManagementDescriptor[5] = { 5, 0x24, 0x01, 0x01, 0x01 };
    static constexpr uint8_t interface0ACMDescriptor[4] = { 4, 0x24, 0x02, 0x00 };
    static constexpr uint8_t interface0UnionDescriptor[5] = { 5, 0x24, 0x06, 0x00, 0x01 };
    static constexpr uint8_t interface0Endpoint1Descriptor[7] = { 7, 0x05, 0x81, 0x03, 8, 0, 0xFF }; //we need this endpoint, but it always NAKs...
    static constexpr uint8_t interface1Descriptor[9] = { 9, 0x04, 1, 0, 2, 0x0A, 0x00, 0, 0 };
    // we use interrupt endpoints for the main data transfer, as they are easier to use, and have limited latency,
    // but this limits our data rate to 64kB/s. If we want more, we can get it using bulk endpoints...
    // it is unlikely that this will become a major issue however, as the Ethernet interface is there for the high-speed stuff.
    static constexpr uint8_t interface1Endpoint2Descriptor[7] = { 7, 0x05, 0x82, 0x03, 64, 0, 1 }; //rx
    static constexpr uint8_t interface1Endpoint3Descriptor[7] = { 7, 0x05, 0x02, 0x03, 64, 0, 1 }; //tx

    static constexpr uint8_t configurationDescriptorAll[67] = { 9, 0x02, 67, 0, 2, 1, 0, 0x80, 0,
            9, 0x04, 0, 0, 1, 0x02, 0x02, 1, 0,
            5, 0x24, 0x00, 0x20, 0x01,
            5, 0x24, 0x01, 0x01, 0x01,
            4, 0x24, 0x02, 0x00,
            5, 0x24, 0x06, 0x00, 0x01,
            7, 0x05, 0x81, 0x03, 8, 0, 0xFF,
            9, 0x04, 1, 0, 2, 0x0A, 0x00, 0, 0,
            7, 0x05, 0x82, 0x03, 64, 0, 1,
            7, 0x05, 0x02, 0x03, 64, 0, 1 };

    UsbRegisters *registers;
    UsbPbm *pbm;
    UsbEndpointRegister endpointRegister;

    uint16_t txBufferStart;
    uint16_t rxBufferStart;

    uint16_t expectedLength = 0;
    bool expectedRxNTx = false;
    bool statusExpected = false;

    bool configured = false;
    bool isSendEncapsulated = false;

    uint32_t lineCodingBaud = 9600;
    uint8_t lineCodingFormat = 0;
    uint8_t lineCodingParity = 0;
    uint8_t lineCodingBits = 8;

    uint8_t descriptorType = 0;
    uint8_t descriptorOffset = 0;

    uint8_t nextAddr = 0;

    void __attribute__ ((noinline)) stallAll() {
        //reset both interrupt sources as well
        expectedLength = 0;
        statusExpected = false;
        endpointRegister.clearRxComplete();
        endpointRegister.clearTxComplete();
        endpointRegister.setEndpointRxStatus(UsbEndpointStall);
        endpointRegister.setEndpointTxStatus(UsbEndpointStall);
    }

    void setupNext() {
        expectedLength = 0;
        statusExpected = false;
        endpointRegister.setEndpointKind(false);
        endpointRegister.clearRxComplete();
        endpointRegister.clearTxComplete();
        ackRx();
        ackTx();
    }

    void receiveStatusNext() {
        pbm->getbufferDescriptor()[0] = txBufferStart * sizeof(uint16_t);
        pbm->getbufferDescriptor()[1] = 0;
        endpointRegister.setEndpointKind(true);
        statusExpected = true;
        expectedRxNTx = true;
        ackRx();
        ackTx();
    }

    void sendStatusNext() {
        pbm->getbufferDescriptor()[0] = txBufferStart * sizeof(uint16_t);
        pbm->getbufferDescriptor()[1] = 0;
        statusExpected = true;
        expectedRxNTx = false;
        ackTx();
        ackRx();
    }

    void ackRx() {
        endpointRegister.setEndpointRxStatus(UsbEndpointValid);
    }

    void ackTx() {
        endpointRegister.setEndpointTxStatus(UsbEndpointValid);
    }

    void nackRx() {
        endpointRegister.setEndpointRxStatus(UsbEndpointNak);
    }

    void nackTx() {
        endpointRegister.setEndpointTxStatus(UsbEndpointNak);
    }

    void __attribute__((noinline)) stallRx() {
        endpointRegister.setEndpointRxStatus(UsbEndpointStall);
    }

    void __attribute__((noinline)) stallTx() {
        endpointRegister.setEndpointTxStatus(UsbEndpointStall);
    }

    uint16_t getRxLength() {
        return pbm->getbufferDescriptor()[3] & 0x3FF;
    }

    void parseSetup() {
        uint8_t bmReqType = pbm->buffer[rxBufferStart] & 0xFF;
        uint8_t bReq = pbm->buffer[rxBufferStart] >> 8;
        uint16_t wValue = pbm->buffer[rxBufferStart + 1];
        uint16_t wIndex = pbm->buffer[rxBufferStart + 2];
        uint16_t wLength = pbm->buffer[rxBufferStart + 3];

        // the following parses bits of the USB setup message... read the USB 2.0 spec for more details.
        if ((bmReqType & 0x80) != 0) {
            expectedRxNTx = false;
        } else {
            expectedRxNTx = true;
        }
        uint8_t type = (bmReqType >> 5) & 0x03;
        uint8_t recipient = bmReqType & 0x1F;
        if (type == 0) {
            //standard request
            UsbStandardControlMessageType reqType = (UsbStandardControlMessageType) bReq;
            switch (reqType) {
            case SET_ADDRESS:
                if (wIndex != 0 || wLength != 0 || recipient != 0) {
                    stallAll();
                    return;
                }
                nextAddr = wValue | 0x80;
                sendStatusNext();
                return;
            case GET_DESCRIPTOR:
                descriptorType = wValue >> 8;
                if (wIndex != 0 || recipient != 0 || (wValue & 0xFF) != 0 || (descriptorType != 1 && descriptorType != 2)) {
                    stallAll();
                    return;
                }
                expectedLength = wLength;
                sendDescriptor();
                return;
            case SET_CONFIGURATION:
                if ((wValue != 0 && wValue != 1) || wIndex != 0 || wLength != 0 || recipient != 0) {
                    stallAll();
                    return;
                }
                configured = wValue != 0;
                sendStatusNext();
                return;
            case GET_CONFIGURATION:
                if (wValue != 0 || wIndex != 0 || wLength != 1 || recipient != 0 || !expectedRxNTx) {
                    stallAll();
                    return;
                }
                pbm->buffer[txBufferStart] = configured ? 1 : 0;
                pbm->getbufferDescriptor()[0] = txBufferStart * sizeof(uint16_t);
                pbm->getbufferDescriptor()[1] = 1;
                expectedLength = 0;
                ackTx();
                ackRx();
                return;
            case GET_STATUS: //simply transmit a zero...
                if (wValue != 0 || wIndex != 0 || wLength != 2 || recipient > 2 || !expectedRxNTx) {
                    stallAll();
                    return;
                }
                pbm->buffer[txBufferStart] = 0;
                pbm->getbufferDescriptor()[0] = txBufferStart * sizeof(uint16_t);
                pbm->getbufferDescriptor()[1] = 2;
                expectedLength = 0;
                ackTx();
                stallRx();
                return;
            default:
                stallAll();
                return;
            }
        } else {
            stallAll();
            return;
        }
    }

    void parseRxData() {
        if (statusExpected && expectedRxNTx) {
            statusExpected = false;
            endpointRegister.setEndpointKind(false);
            setupNext();
            return;
        } else if (statusExpected) {
            sendStatusNext();
            return;
        }
        uint16_t length = getRxLength();
        if (length == expectedLength) {
            sendStatusNext();
        }
        if (length > expectedLength) {
            stallAll();
            return;
        }
        if (length < expectedLength) {
            expectedLength -= length;
        }
        // Do something with the data... we need to flush the buffer whether this is the last packet or not...
        // if there is an error, just stallAll and return

        if (!isSendEncapsulated) {
            // We must be in the SET_LINE_CODING command, as it is the only one we transmit for - this has only one packet of data - great!!
            lineCodingBaud = pbm->buffer[rxBufferStart] | (pbm->buffer[rxBufferStart + 1] << 16);
            lineCodingFormat = pbm->buffer[rxBufferStart + 2] & 0xFF;
            lineCodingParity = pbm->buffer[rxBufferStart + 2] >> 8;
            lineCodingBits = pbm->buffer[rxBufferStart + 3] & 0xFF;
        }
        if (length != expectedLength) {
            ackRx();
            stallTx();
        }
    }

    void sendDescriptor() {
        uint8_t lengthToSend = maxPacketLength;
        if (expectedLength < maxPacketLength) {
            lengthToSend = expectedLength;
        }
        const uint8_t *data = NULL;
        if (descriptorType == 1) {
            data = deviceDescriptor;
        } else {
            data = configurationDescriptorAll;
        }
        if (descriptorType == 1 && lengthToSend > sizeof(deviceDescriptor)) {
            lengthToSend = sizeof(deviceDescriptor);
            expectedLength = lengthToSend;
        } else if (descriptorType == 2 && lengthToSend > sizeof(configurationDescriptorAll)) {
            lengthToSend = sizeof(configurationDescriptorAll);
            expectedLength = lengthToSend;
        }
        expectedLength -= lengthToSend;
        bool posInHW = false;
        for (descriptorOffset = 0; descriptorOffset < lengthToSend; descriptorOffset++) {
            if (posInHW) {
                pbm->buffer[txBufferStart + descriptorOffset / sizeof(uint16_t)] |= data[descriptorOffset] << 8;
            } else {
                pbm->buffer[txBufferStart + descriptorOffset / sizeof(uint16_t)] = data[descriptorOffset];
            }
            posInHW = !posInHW;
        }
        pbm->getbufferDescriptor()[0] = txBufferStart * sizeof(uint16_t);
        pbm->getbufferDescriptor()[1] = lengthToSend;
        ackTx();
        ackRx();
    }

    void sentTxData() {
        if (statusExpected && !expectedRxNTx) {
            statusExpected = false;
            endpointRegister.setEndpointKind(false);
            setupNext();
            if (nextAddr != 0) {
                registers->DADDR = nextAddr;
                nextAddr = 0;
            }
            return;
        } else if (statusExpected) {
            receiveStatusNext();
            return;
        }

        if (expectedLength == 0 || descriptorType == 1) {
            receiveStatusNext();
            return;
        }
        uint16_t lengthToSend = maxPacketLength;
        if (expectedLength < maxPacketLength) {
            lengthToSend = expectedLength;
        }
        // Generate the new data to send...
        // if there is an error, just stallAll and return

        //the only multi-packet transmit we do is device descriptors, so just assume we're doing those...
        const uint8_t *data = NULL;
        if (descriptorType == 1) {
            data = deviceDescriptor;
        } else {
            data = configurationDescriptorAll;
        }
        if (descriptorType == 1 && lengthToSend + descriptorOffset > sizeof(deviceDescriptor)) {
            lengthToSend = sizeof(deviceDescriptor) - descriptorOffset;
            expectedLength = lengthToSend;
        } else if (descriptorType == 2 && lengthToSend + descriptorOffset > sizeof(configurationDescriptorAll)) {
            lengthToSend = sizeof(configurationDescriptorAll) - descriptorOffset;
            expectedLength = lengthToSend;
        }
        expectedLength -= lengthToSend;
        bool posInHW = false;
        for (int i = 0; i < lengthToSend; descriptorOffset++, i++) {
            if (posInHW) {
                pbm->buffer[txBufferStart + i / 2] |= data[descriptorOffset] << 8;
            } else {
                pbm->buffer[txBufferStart + i / 2] = data[descriptorOffset];
            }
            posInHW = !posInHW;
        }
        pbm->getbufferDescriptor()[0] = txBufferStart * sizeof(uint16_t);
        pbm->getbufferDescriptor()[1] = lengthToSend;

        ackRx();
        ackTx();
    }

    void packetReceived() {
        nackRx();
        nackTx();
        if (endpointRegister.getSetupLast()) {
            parseSetup();
            endpointRegister.clearRxComplete();
        } else {
            if (expectedRxNTx && endpointRegister.getRxComplete()) {
                parseRxData();
                endpointRegister.clearRxComplete();
            } else if (!expectedRxNTx && endpointRegister.getTxComplete()) {
                sentTxData();
                endpointRegister.clearTxComplete();
            } else {
                stallAll();
            }
        }
    }

public:
    void setup(UsbRegisters *registers, UsbPbm *pbm, volatile uint16_t *endpointRegister, uint16_t txBufferStart, uint16_t rxBufferStart) {
        this->registers = registers;
        this->pbm = pbm;
        this->endpointRegister = UsbEndpointRegister(endpointRegister);
        this->txBufferStart = txBufferStart;
        this->rxBufferStart = rxBufferStart;
        this->endpointRegister.resetAll();
        this->endpointRegister.setEndpointType(UsbControlEndpoint);
        expectedLength = 0;
        expectedRxNTx = false;
        statusExpected = false;
        configured = false;
        isSendEncapsulated = false;
        descriptorType = 1;
        descriptorOffset = 0;
        nextAddr = 0;
        setupNext();

        const uint16_t rxBlockSize32Byte = 0x8000;
        const uint16_t rx2Blocks = 0x0400;

        pbm->getbufferDescriptor()[0] = txBufferStart * sizeof(uint16_t);
        pbm->getbufferDescriptor()[1] = 0;
        pbm->getbufferDescriptor()[2] = rxBufferStart * sizeof(uint16_t);
        pbm->getbufferDescriptor()[3] = rxBlockSize32Byte | rx2Blocks; // give us 64 bytes of input - should be enough.
    }

    void interrupt() {
        packetReceived();
    }
};

template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::deviceDescriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::configurationDescriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface0Descriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface0HeaderDescriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface0CallManagementDescriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface0ACMDescriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface0UnionDescriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface0Endpoint1Descriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface1Descriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface1Endpoint2Descriptor[];
template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::interface1Endpoint3Descriptor[];

template<class LL>
constexpr uint8_t UsbDefaultEndpoint<LL>::configurationDescriptorAll[];

#endif /* SRC_MAIN_CPP_ARM_NO_OS_STM32G4_USB_MODULE_LOWLEVEL_SPECIFIC_USBDEFAULTENDPOINT_HPP_ */

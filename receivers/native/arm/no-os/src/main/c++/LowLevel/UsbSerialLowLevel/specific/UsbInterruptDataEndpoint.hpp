/*
 * UsbInterruptDataEndpoint.hpp
 *
 *  Created on: 26 Jul 2021
 *      Author: robert
 */

#ifndef LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERRUPTDATAENDPOINT_HPP_
#define LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERRUPTDATAENDPOINT_HPP_

#include <LowLevel/llIncludes.hpp>
#include <LowLevel/UartLowLevel/SerialRingBuffer.hpp>
#include "UsbRegisters.hpp"
#include "UsbEndpointRegister.hpp"

template<class LL>
class UsbInterruptDataEndpoint {

    static const uint16_t maxPacketLength = 64;

    void (*volatile targetValueCallback)() = NULL;
    uint8_t targetValue = 0;

    UsbPbm *pbm;
    UsbEndpointRegister endpointRegister;
    SerialRingBuffer<LL::UsbBufferRxSize> rxBuffer;
    SerialRingBuffer<LL::UsbBufferTxSize> txBuffer;

    uint16_t txPbmBufferStart;
    uint16_t rxPbmBufferStart;

    uint8_t endpointIndex;

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
    void stallRx() {
        endpointRegister.setEndpointRxStatus(UsbEndpointStall);
    }
    void stallTx() {
        endpointRegister.setEndpointTxStatus(UsbEndpointStall);
    }
// we don't use DMAs because setting them up would take longer than iterating the transmit loop a few times
// 64 bytes isn't much, and it doesn't happen that often.
    void __attribute__ ((noinline)) parseRxData() {
        const uint16_t rxByteCountMask = 0x3FF;

        uint16_t rxLength = pbm->getbufferDescriptor()[4 * endpointIndex + 3] & rxByteCountMask;
        uint8_t *data = (uint8_t*) (pbm->buffer + rxPbmBufferStart);
        if (targetValueCallback != NULL) {
            for (int i = 0; i < rxLength; ++i) {
                if (data[i] == targetValue) {
                    targetValueCallback();
                }
            }
        }
        rxBuffer.write(data, rxLength);
        if (rxBuffer.canWrite(maxPacketLength)) {
            ackRx();
        }
    }
    void __attribute__ ((noinline)) sentTxData() {
        uint16_t txLength = txBuffer.read16(pbm->buffer + txPbmBufferStart, maxPacketLength);
        pbm->getbufferDescriptor()[4 * endpointIndex + 1] = txLength;
        if (txLength != 0) {
            ackTx();
        }
    }
public:

    UsbInterruptDataEndpoint() :
            pbm(NULL), endpointRegister(NULL), txPbmBufferStart(0), rxPbmBufferStart(0), endpointIndex(0) {
    }

    void setup(UsbPbm *pbm, volatile uint16_t *endpointRegister, uint16_t txBufferStart, uint16_t rxBufferStart, uint8_t endpointIndex) {
        this->pbm = pbm;
        this->endpointRegister = UsbEndpointRegister(endpointRegister);
        this->txPbmBufferStart = txBufferStart;
        this->rxPbmBufferStart = rxBufferStart;
        this->endpointIndex = endpointIndex;
        this->endpointRegister.resetAll();
        this->endpointRegister.setEndpointAddress(endpointIndex);
        this->endpointRegister.setEndpointType(UsbInterruptEndpoint);
        nackTx();
        nackRx();

        checkBuffers();

        const uint16_t rxBlockSize32Byte = 0x8000;
        const uint16_t rx2Blocks = 0x0400;

        pbm->getbufferDescriptor()[endpointIndex * 4 + 0] = txBufferStart * sizeof(uint16_t);
        pbm->getbufferDescriptor()[endpointIndex * 4 + 1] = 0;
        pbm->getbufferDescriptor()[endpointIndex * 4 + 2] = rxBufferStart * sizeof(uint16_t);
        pbm->getbufferDescriptor()[endpointIndex * 4 + 3] = rxBlockSize32Byte | rx2Blocks; // give us 64 bytes of input space - the maximum packet size
    }
    void setTargetValue(void (*volatile targetValueCallback)(), uint8_t targetValue) {
        this->targetValueCallback = targetValueCallback;
        this->targetValue = targetValue;
    }
    void clearTargetValue() {
        this->targetValueCallback = NULL;
        this->targetValue = 0;
    }
    void interrupt() {
        if (endpointRegister.getRxComplete()) {
            parseRxData();
            endpointRegister.clearRxComplete();
        } else if (endpointRegister.getTxComplete()) {
            sentTxData();
            endpointRegister.clearTxComplete();
        }
    }
    void checkBuffers() {
        if (txBuffer.available() > 0) {
            sentTxData();
            ackTx();
        }
        if (rxBuffer.canWrite(maxPacketLength)) {
            ackRx();
        }
    }
    SerialRingBuffer<LL::UsbBufferRxSize>* getRxBuffer() {
        return &rxBuffer;
    }
    SerialRingBuffer<LL::UsbBufferTxSize>* getTxBuffer() {
        return &txBuffer;
    }
};

#endif /* LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERRUPTDATAENDPOINT_HPP_ */

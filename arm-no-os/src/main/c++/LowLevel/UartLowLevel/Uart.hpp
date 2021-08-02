/*
 * Uart.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_
#include "specific/UartInternal.hpp"
#include "../DmaLowLevel/Dma.hpp"
#include "../DmaLowLevel/DmaManager.hpp"
#include "../GeneralLLSetup.hpp"
#include "Serial.hpp"
#include "SerialRingBuffer.hpp"

class UartManager;
class UartInterruptManager;
//always does 8 bit bytes, no parity - just for simplicity
class Uart: public Serial {
private:
    void (*volatile targetValueCallback)(SerialIdentifier);
    void (*rxOverflowCallback)(SerialIdentifier);
    SerialRingBuffer<GeneralHalSetup::UartBufferRxSize> rxBuffer;
    SerialRingBuffer<GeneralHalSetup::UartBufferTxSize> txBuffer;
    UartInternal uart;
    DmaMuxRequest requestTx = DMAMUX_NO_MUX;
    Dma *txDma = NULL;
    uint16_t dmaStartDist = 0;
    uint16_t availableData = 0;
    uint16_t peekDist = 0;
    uint8_t targetValue = 0;

    friend void UartDmaCallback(Dma*, DmaTerminationStatus);
    friend void UartTxOverflowCallback(SerialIdentifier);
    friend UartManager;
    friend UartInterruptManager;

    void interrupt();

    void dmaInterrupt(DmaTerminationStatus status);

    void txOverflowInterrupt();

public:
    void setUart(UartInternal uart, Dma *txDma, DmaMuxRequest requestTx) {
        this->uart = uart;
        this->txDma = txDma;
        this->requestTx = requestTx;

    }
    virtual void init(void (*volatile bufferOverflowCallback)(SerialIdentifier), uint32_t baud_rate, bool singleNdoubleStop);
    //the buffer overflow handler can read/skip in the callback to clear space - if it doesn't clear enough space we abort the write

    virtual void setTargetValue(void (*volatile targetValueCallback)(SerialIdentifier), uint8_t targetValue) {
        this->targetValueCallback = targetValueCallback;
        this->targetValue = targetValue;
    }
    virtual void clearTargetValue() {
        this->targetValueCallback = NULL;
    }
    virtual SerialIdentifier getId() {
        return uart.getId();
    }
    virtual bool write(uint8_t datum) {
        return txBuffer.write(datum);
    }

    virtual bool write(const uint8_t *buffer, uint16_t length) {
        return txBuffer.write(buffer, length);
    }
    virtual bool canWrite(uint16_t length) {
        return txBuffer.canWrite(length);
    }

    virtual bool attemptWrite(const uint8_t *buffer, uint16_t length) { //starts writing without allowing read yet - used to discover if a message will fit
        return txBuffer.attemptWrite(buffer, length);
    }
    virtual bool attemptWrite(uint8_t datum) {
        return txBuffer.attemptWrite(datum);
    }

    virtual bool canAttemptWrite(uint16_t length) {
        return txBuffer.canAttemptWrite(length);
    }

    virtual void cancelWrite() { //undoes an attempted write
        txBuffer.cancelWrite();
    }
    virtual void completeWrite() { //allows reading of any attempted write
        txBuffer.completeWrite();
    }
    virtual void transmitWriteBuffer();

    virtual void clearRxFifo();

    virtual SerialError getError(uint16_t length);

    virtual uint16_t skip(uint16_t length);
    virtual uint16_t available() {
        clearRxFifo();
        return availableData;
    }
    virtual uint16_t read(uint8_t *buffer, uint16_t length);
    virtual int16_t read(); //-1 if no data

    virtual uint16_t availablePeek();
    virtual uint16_t peek(uint8_t *buffer, uint16_t length);
    virtual int16_t peek(); //-1 if no data

    virtual void resetPeek();
    virtual void skipToPeek();

    virtual int32_t getDistance(uint8_t value);  //returns the number of bytes until the specified value appears, including the value

};
#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_ */

/*
 * Uart.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "specific/UartInternal.hpp"
#include <arm-no-os/system/dma/DmaManager.hpp>
#include "Serial.hpp"
#include "SerialRingBuffer.hpp"

template<class LL>
class UartManager;

template<class LL>
void UartDmaCallback(Dma<LL> *dma, DmaTerminationStatus status);

template<class LL>
void UartTxOverflowCallback(SerialIdentifier id);

//always does 8 bit bytes, no parity - just for simplicity
template<class LL>
class Uart: public Serial {
    //TODO: Use a dma for Rx as well?
private:
    void (*volatile targetValueCallback)(SerialIdentifier);
    void (*rxOverflowCallback)(SerialIdentifier);
    SerialRingBuffer<LL::UartBufferRxSize> rxBuffer;
    SerialRingBuffer<LL::UartBufferTxSize> txBuffer;
    UartInternal<LL> uart;
    DmaMuxRequest requestTx = DMAMUX_NO_MUX;
    Dma<LL> *txDma = NULL;
    uint16_t dmaStartDist = 0;
    uint16_t availableData = 0;
    uint16_t peekDist = 0;
    uint8_t targetValue = 0;

    friend void UartDmaCallback<LL>(Dma<LL>*, DmaTerminationStatus);
    friend void UartTxOverflowCallback<LL>(SerialIdentifier);
    friend UartManager<LL> ;

    void interrupt();

    void dmaInterrupt(DmaTerminationStatus status);

    void txOverflowInterrupt();

public:
    void setUart(UartInternal<LL> uart, Dma<LL> *txDma, DmaMuxRequest requestTx) {
        this->id = uart.getId();
        this->uart = uart;
        this->txDma = txDma;
        this->requestTx = requestTx;
    }
    void setUart(UartInternal<LL> uart, Dma<LL> *txDma) {
        this->id = uart.getId();
        this->uart = uart;
        this->txDma = txDma;
        this->requestTx = DMAMUX_NO_MUX;
    }
    uint32_t getMaxBaud() {
        return ClockManager<LL>::getClock(SysClock)->getFreqKhz() * 1000 / 16;
    }
    uint32_t getMinBaud() {
        return 0;
    }
    void init(void (*volatile bufferOverflowCallback)(SerialIdentifier), uint32_t baud_rate, bool singleNdoubleStop);
    //the buffer overflow handler can read/skip in the callback to clear space - if it doesn't clear enough space we abort the write

    void setTargetValue(void (*volatile targetValueCallback)(SerialIdentifier), uint8_t targetValue) {
        this->targetValueCallback = targetValueCallback;
        this->targetValue = targetValue;
    }

    void clearTargetValue() {
        this->targetValueCallback = NULL;
    }

    bool write(uint8_t datum) {
        return txBuffer.write(datum);
    }

    bool write(const uint8_t *buffer, uint16_t length) {
        return txBuffer.write(buffer, length);
    }

    bool canWrite(uint16_t length) {
        return txBuffer.canWrite(length);
    }

    bool attemptWrite(const uint8_t *buffer, uint16_t length) { //starts writing without allowing read yet - used to discover if a message will fit
        return txBuffer.attemptWrite(buffer, length);
    }

    bool attemptWrite(uint8_t datum) {
        return txBuffer.attemptWrite(datum);
    }

    bool canAttemptWrite(uint16_t length) {
        return txBuffer.canAttemptWrite(length);
    }

    void cancelWrite() { //undoes an attempted write
        txBuffer.cancelWrite();
    }

    void completeWrite() { //allows reading of any attempted write
        txBuffer.completeWrite();
    }

    void transmitWriteBuffer();

    void clearRxFifo();

    SerialError getError(uint16_t length);

    uint16_t skip(uint16_t length);

    uint16_t available() {
        clearRxFifo();
        return availableData;
    }

    uint16_t read(uint8_t *buffer, uint16_t length);

    int16_t read(); //-1 if no data

    uint16_t availablePeek();

    uint16_t peek(uint8_t *buffer, uint16_t length);

    int16_t peek(); //-1 if no data

    void resetPeek();

    void skipToPeek();

    int32_t getDistance(uint8_t value);  //returns the number of bytes until the specified value appears, including the value
};
#include "specific/Uartcpp.hpp"
#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_UART_HPP_ */

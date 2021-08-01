/*
 * Usb.hpp
 *
 *  Created on: 24 Jul 2021
 *      Author: robert
 */

#ifndef LOWLEVEL_USBSERIALLOWLEVEL_USB_HPP_
#define LOWLEVEL_USBSERIALLOWLEVEL_USB_HPP_
#include "../GeneralLLSetup.hpp"
#include "../UartLowLevel/UartRingBuffer.hpp"
#include "../UartLowLevel/Uart.hpp"
#include "specific/UsbRegisters.hpp"
#include "specific/UsbInternal.hpp"

class Usb {
    static Usb *usb;
    UsbInternal internal;
    void (*volatile targetValueCallback)(UartIdentifier) = NULL;
    friend void UsbTargetValueCallback();

    UartIdentifier id;

public:
    Usb(UartIdentifier id) :
            id(id) {
        usb = this;
    }

    void init(void (*volatile bufferOverflowCallback)(UartIdentifier), uint32_t baud_rate, bool singleNdoubleStop) {
        // we do nothing with these arguments, they are merely for Uart compatibility...
        internal.setup();
    }

    void setTargetValue(void (*volatile targetValueCallback)(UartIdentifier), uint8_t targetValue);

    void clearTargetValue() {
        internal.clearTargetValue();
    }
    UartIdentifier getId() {
        return id;
    }
    bool write(uint8_t datum) {
        return internal.getTxBuffer()->write(datum);
    }

    bool write(const uint8_t *buffer, uint16_t length) {
        return internal.getTxBuffer()->write(buffer, length);
    }
    bool canWrite(uint16_t length) {
        return internal.getTxBuffer()->canWrite(length);
    }

    bool attemptWrite(const uint8_t *buffer, uint16_t length) { //starts writing without allowing read yet - used to discover if a message will fit
        return internal.getTxBuffer()->attemptWrite(buffer, length);
    }
    bool attemptWrite(uint8_t datum) {
        return internal.getTxBuffer()->attemptWrite(datum);
    }

    bool canAttemptWrite(uint16_t length) {
        return internal.getTxBuffer()->canAttemptWrite(length);
    }

    void cancelWrite() { //undoes an attempted write
        internal.getTxBuffer()->cancelWrite();
    }
    void completeWrite() { //allows reading of any attempted write
        internal.getTxBuffer()->completeWrite();
    }
    void transmitWriteBuffer() {
        internal.checkBuffers();
    }

    UartError getError(uint16_t length) {
        return UartNoError;
    }

    uint16_t skip(uint16_t length) {
        uint16_t result = internal.getRxBuffer()->skip(length);
        internal.checkBuffers();
        return result;
    }
    uint16_t available() {
        return internal.getRxBuffer()->available();
    }
    uint16_t read(uint8_t *buffer, uint16_t length) {
        uint16_t result = internal.getRxBuffer()->read(buffer, length);
        internal.checkBuffers();
        return result;
    }
    int16_t read() { //-1 if no data
        int16_t result = internal.getRxBuffer()->read();
        internal.checkBuffers();
        return result;
    }

    int32_t getDistance(uint8_t value) {  //returns the number of bytes until the specified value appears, including the value
        int16_t i = 1;
        internal.getRxBuffer()->resetPeek();
        int16_t val = internal.getRxBuffer()->peek();
        while (val != value && val != -1) {
            val = internal.getRxBuffer()->peek();
            i++;
        }
        internal.getRxBuffer()->resetPeek();
        if (val == -1) {
            return -1;
        } else {
            return i;
        }
    }
};

#endif /* LOWLEVEL_USBSERIALLOWLEVEL_USB_HPP_ */

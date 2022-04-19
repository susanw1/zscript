/*
 * Usb.hpp
 *
 *  Created on: 24 Jul 2021
 *      Author: robert
 */

#ifndef LOWLEVEL_USBSERIALLOWLEVEL_USB_HPP_
#define LOWLEVEL_USBSERIALLOWLEVEL_USB_HPP_

#include "../GeneralLLSetup.hpp"
#include "../UartLowLevel/Serial.hpp"
#include "../UartLowLevel/SerialRingBuffer.hpp"
#include "specific/UsbRegisters.hpp"
#include "specific/UsbInternal.hpp"

class Usb: public Serial {
    private:
        static Usb *usb;
        UsbInternal internal;
        void (*volatile targetValueCallback)(SerialIdentifier) = NULL;
        friend void UsbTargetValueCallback();

        SerialIdentifier id;

    public:
        Usb(SerialIdentifier id) :
                id(id) {
            usb = this;
        }

        virtual void init(void (*volatile bufferOverflowCallback)(SerialIdentifier), uint32_t baud_rate, bool singleNdoubleStop) {
            // we do nothing with these arguments, they are merely for Uart compatibility...
            internal.setup();
        }

        virtual void setTargetValue(void (*volatile targetValueCallback)(SerialIdentifier), uint8_t targetValue);

        virtual void clearTargetValue() {
            internal.clearTargetValue();
        }

        virtual SerialIdentifier getId() {
            return id;
        }

        virtual bool write(uint8_t datum) {
            return internal.getTxBuffer()->write(datum);
        }

        virtual bool write(const uint8_t *buffer, uint16_t length) {
            return internal.getTxBuffer()->write(buffer, length);
        }

        virtual bool canWrite(uint16_t length) {
            return internal.getTxBuffer()->canWrite(length);
        }

        virtual bool attemptWrite(const uint8_t *buffer, uint16_t length) { //starts writing without allowing read yet - used to discover if a message will fit
            return internal.getTxBuffer()->attemptWrite(buffer, length);
        }

        virtual bool attemptWrite(uint8_t datum) {
            return internal.getTxBuffer()->attemptWrite(datum);
        }

        virtual bool canAttemptWrite(uint16_t length) {
            return internal.getTxBuffer()->canAttemptWrite(length);
        }

        virtual void cancelWrite() { //undoes an attempted write
            internal.getTxBuffer()->cancelWrite();
        }

        virtual void completeWrite() { //allows reading of any attempted write
            internal.getTxBuffer()->completeWrite();
        }

        virtual void transmitWriteBuffer() {
            internal.checkBuffers();
        }

        virtual SerialError getError(uint16_t length) {
            return SerialNoError;
        }

        virtual uint16_t skip(uint16_t length) {
            uint16_t result = internal.getRxBuffer()->skip(length);
            internal.checkBuffers();
            return result;
        }

        virtual uint16_t available() {
            return internal.getRxBuffer()->available();
        }

        virtual uint16_t read(uint8_t *buffer, uint16_t length) {
            uint16_t result = internal.getRxBuffer()->read(buffer, length);
            internal.checkBuffers();
            return result;
        }

        virtual int16_t read() { //-1 if no data
            int16_t result = internal.getRxBuffer()->read();
            internal.checkBuffers();
            return result;
        }

        virtual uint16_t availablePeek() {
            return internal.getRxBuffer()->availablePeek();
        }

        virtual uint16_t peek(uint8_t *buffer, uint16_t length) {
            return internal.getRxBuffer()->peek(buffer, length);
        }

        virtual int16_t peek() { //-1 if no data
            return internal.getRxBuffer()->peek();
        }

        virtual void resetPeek() {
            internal.getRxBuffer()->resetPeek();
        }

        virtual void skipToPeek() {
            internal.getRxBuffer()->skipToPeek();
            internal.checkBuffers();
        }

        virtual int32_t getDistance(uint8_t value) {  //returns the number of bytes until the specified value appears, including the value
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

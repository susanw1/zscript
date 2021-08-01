/*
 * UsbInternal.hpp
 *
 *  Created on: 24 Jul 2021
 *      Author: robert
 */

#ifndef LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERNAL_HPP_
#define LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERNAL_HPP_
#include "../../GeneralLLSetup.hpp"
#include "../../ClocksLowLevel/SystemMilliClock.hpp"
#include "../../GpioLowLevel/Gpio.hpp"
#include "../../GpioLowLevel/GpioManager.hpp"
#include "UsbRegisters.hpp"

#include "UsbDefaultEndpoint.hpp"
#include "UsbInterruptDataEndpoint.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void USB_LP_IRQHandler();
void USB_HP_IRQHandler();
#ifdef __cplusplus
}
#endif

class UsbInternal {
    static UsbInternal *usbI;
    UsbDefaultEndpoint defaultEndpoint;
    UsbInterruptDataEndpoint dataEndpoint;
    UsbRegisters *registers;
    UsbEndpointRegisters *endpointRegisters;
    UsbPbm *pbm;
    friend void USB_LP_IRQHandler();
    friend void USB_HP_IRQHandler();

    void interrupt() {

        if ((registers->ISTR & 0x0400) != 0) { // we are in a reset... do that thing!
            reset();
        } else if ((registers->ISTR & 0x0F) == 0) {
            defaultEndpoint.interrupt();
        } else {
            dataEndpoint.interrupt();
        }
    }

public:
    UsbInternal() :
            registers(NULL), endpointRegisters(NULL), pbm(NULL) {
        usbI = this;
    }
    void setup() {
        SystemMilliClock::blockDelayMillis(1);
        NVIC_SetPriority(USB_LP_IRQn, 1);
        NVIC_DisableIRQ(USB_LP_IRQn);

        NVIC_SetPriority(USB_HP_IRQn, 1);
        NVIC_DisableIRQ(USB_HP_IRQn);

        GpioManager::activateClock(PA_11);
        GpioManager::activateClock(PA_12);
        GpioManager::getPin(PA_11)->init();
        GpioManager::getPin(PA_11)->setMode(Output);
        GpioManager::getPin(PA_11)->setOutputSpeed(VeryHighSpeed);
        GpioManager::getPin(PA_11)->setOutputMode(PushPull);
        GpioManager::getPin(PA_12)->init();
        GpioManager::getPin(PA_12)->setMode(Output);
        GpioManager::getPin(PA_12)->setOutputSpeed(VeryHighSpeed);
        GpioManager::getPin(PA_12)->setOutputMode(PushPull);

        ClockManager::getClock(HSI48)->set(48000, NONE);
        RCC->APB1ENR1 |= 0x00800100;
        *((uint32_t*) 0x40002000) |= 0x60; //enable the CRS system for clock generation, with auto trim enabled
        RCC->CCIPR &= ~0x0C000000;
        SystemMilliClock::blockDelayMillis(1);

        registers = (UsbRegisters*) 0x40005C40;
        endpointRegisters = (UsbEndpointRegisters*) 0x40005C00;
        pbm = (UsbPbm*) 0x40006000;
        registers->CNTR = 0x8402;
        SystemMilliClock::blockDelayMillis(1);
        registers->CNTR = 0x8400;
        SystemMilliClock::blockDelayMillis(5);
        registers->ISTR = 0x0000;
        registers->BTABLE = 0x0000;
        registers->LPMCSR = 0x0000;

        defaultEndpoint.setup(registers, pbm, (volatile uint16_t*) &(endpointRegisters->EP0R), 32, 96);

        UsbEndpointRegister ep1r((volatile uint16_t*) &endpointRegisters->EP1R);
        ep1r.resetAll();
        ep1r.setEndpointType(UsbInterruptEndpoint);
        ep1r.setEndpointAddress(1);
        ep1r.setEndpointTxStatus(UsbEndpointNak); //minimal setup for endpoint 1, to just NAK everything...

        dataEndpoint.setup(pbm, (volatile uint16_t*) &(endpointRegisters->EP2R), 160, 224, 2);

        registers->BCDR = 0x8000;
        registers->DADDR = 0x0080;
        SystemMilliClock::blockDelayMillis(1);
        registers->CNTR |= 0x0010;
        SystemMilliClock::blockDelayMillis(5);
        registers->CNTR &= ~0x0010;
        NVIC_EnableIRQ(USB_HP_IRQn);
        NVIC_EnableIRQ(USB_LP_IRQn);
    }

    void reset() {
        registers->CNTR = 0x8400;
        registers->ISTR = 0x0000;
        registers->BTABLE = 0x0000;
        registers->LPMCSR = 0x0000;
        defaultEndpoint.setup(registers, pbm, (volatile uint16_t*) &(endpointRegisters->EP0R), 32, 96);

        endpointRegisters->EP1R = ((endpointRegisters->EP1R) & 0x0000) | (((endpointRegisters->EP1R) ^ 0x0000) & 0x7070);
        endpointRegisters->EP1R |= 0x0621; //minimal setup for endpoint 1, to just NAK everything...

        dataEndpoint.setup(pbm, (volatile uint16_t*) &(endpointRegisters->EP2R), 160, 224, 2);

        registers->BCDR = 0x8000;
        registers->DADDR = 0x0080;
    }

    void setTargetValue(void (*volatile targetValueCallback)(), uint8_t targetValue) {
        dataEndpoint.setTargetValue(targetValueCallback, targetValue);
    }
    void clearTargetValue() {
        dataEndpoint.clearTargetValue();
    }
    void checkBuffers() {
        dataEndpoint.checkBuffers();
    }
    UartRingBuffer<GeneralHalSetup::UsbBufferRxSize>* getRxBuffer() {
        return dataEndpoint.getRxBuffer();
    }
    UartRingBuffer<GeneralHalSetup::UsbBufferTxSize>* getTxBuffer() {
        return dataEndpoint.getTxBuffer();
    }
};

#endif /* LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERNAL_HPP_ */

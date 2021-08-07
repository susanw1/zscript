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

void UsbDataTxOverflowCallback(SerialIdentifier);
class UsbInternal {
    static UsbInternal *usbI;
    UsbDefaultEndpoint defaultEndpoint;
    UsbInterruptDataEndpoint dataEndpoint;
    UsbRegisters *registers;
    UsbEndpointRegisters *endpointRegisters;
    UsbPbm *pbm;
    friend void USB_LP_IRQHandler();
    friend void USB_HP_IRQHandler();
    friend void UsbDataTxOverflowCallback(SerialIdentifier);

    void interrupt() {
        const uint16_t resetRequested = 0x0400;
        const uint16_t dataReceived = 0x8000;
        const uint16_t receivedEndpointMask = 0x0F;
        if ((registers->ISTR & resetRequested) != 0) { // we are in a reset... do that thing!
            reset();
        } else if ((registers->ISTR & dataReceived) != 0) {
            if ((registers->ISTR & receivedEndpointMask) == 0) {
                defaultEndpoint.interrupt();
            } else {
                dataEndpoint.interrupt();
            }
        } else {
            registers->ISTR = 0;
        }
    }

public:
    UsbInternal() :
            registers(NULL), endpointRegisters(NULL), pbm(NULL) {
        usbI = this;
    }
    void setup() {
        const uint32_t clockRecoveryRegisterClockEnable = 0x100;
        const uint32_t usbRegisterClockEnable = 0x00800000;
        const uint32_t clockRecoveryEnable = 0x20;
        const uint32_t clockRecoveryAutoTuneEnable = 0x40;

        const uint32_t usbClockSelectMask = 0x0C000000;

        const uint16_t transferCompleteInterruptEnable = 0x8000;
        const uint16_t resetInterruptEnable = 0x0400;
        const uint16_t usbPowerDisable = 0x0002;

        const uint16_t dpPullUpEnable = 0x8000;
        const uint16_t enableReception = 0x0080;

        const uint16_t sendResume = 0x0010;

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
        RCC->APB1ENR1 |= usbRegisterClockEnable | clockRecoveryRegisterClockEnable;
        *((uint32_t*) 0x40002000) |= clockRecoveryAutoTuneEnable | clockRecoveryEnable; //enable the CRS system for clock generation, with auto trim enabled
        RCC->CCIPR &= ~usbClockSelectMask;
        SystemMilliClock::blockDelayMillis(1);

        registers = (UsbRegisters*) 0x40005C40;
        endpointRegisters = (UsbEndpointRegisters*) 0x40005C00;
        pbm = (UsbPbm*) 0x40006000;

        registers->CNTR = transferCompleteInterruptEnable | resetInterruptEnable | usbPowerDisable;
        SystemMilliClock::blockDelayMillis(1);
        registers->CNTR = transferCompleteInterruptEnable | resetInterruptEnable;
        SystemMilliClock::blockDelayMillis(5);
        registers->ISTR = 0;
        registers->BTABLE = 0;
        registers->LPMCSR = 0;

        defaultEndpoint.setup(registers, pbm, (volatile uint16_t*) &(endpointRegisters->EP0R), 32, 96);

        UsbEndpointRegister ep1r((volatile uint16_t*) &endpointRegisters->EP1R);
        ep1r.resetAll();
        ep1r.setEndpointType(UsbInterruptEndpoint);
        ep1r.setEndpointAddress(1);
        ep1r.setEndpointTxStatus(UsbEndpointNak); //minimal setup for endpoint 1, to just NAK everything...

        dataEndpoint.setup(pbm, (volatile uint16_t*) &(endpointRegisters->EP2R), 160, 224, 2);

        registers->BCDR = dpPullUpEnable;
        registers->DADDR = enableReception;
        SystemMilliClock::blockDelayMillis(1);
        registers->CNTR |= sendResume;
        SystemMilliClock::blockDelayMillis(5);
        registers->CNTR &= ~sendResume;
        NVIC_EnableIRQ(USB_HP_IRQn);
        NVIC_EnableIRQ(USB_LP_IRQn);
        dataEndpoint.getTxBuffer()->setCallback(&UsbDataTxOverflowCallback, GeneralHalSetup::UsbSerialId);
    }

    void reset() {
        const uint16_t transferCompleteInterruptEnable = 0x8000;
        const uint16_t resetInterruptEnable = 0x0400;

        const uint16_t dpPullUpEnable = 0x8000;
        const uint16_t enableReception = 0x0080;

        registers->CNTR = transferCompleteInterruptEnable | resetInterruptEnable;
        registers->ISTR = 0;
        registers->BTABLE = 0;
        registers->LPMCSR = 0;
        defaultEndpoint.setup(registers, pbm, (volatile uint16_t*) &(endpointRegisters->EP0R), 32, 96);

        endpointRegisters->EP1R = ((endpointRegisters->EP1R) & 0x0000) | (((endpointRegisters->EP1R) ^ 0x0000) & 0x7070);
        endpointRegisters->EP1R |= 0x0621; //minimal setup for endpoint 1, to just NAK everything...

        dataEndpoint.setup(pbm, (volatile uint16_t*) &(endpointRegisters->EP2R), 160, 224, 2);

        registers->BCDR = dpPullUpEnable;
        registers->DADDR = enableReception;
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
    SerialRingBuffer<GeneralHalSetup::UsbBufferRxSize>* getRxBuffer() {
        return dataEndpoint.getRxBuffer();
    }
    SerialRingBuffer<GeneralHalSetup::UsbBufferTxSize>* getTxBuffer() {
        return dataEndpoint.getTxBuffer();
    }
};

#endif /* LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERNAL_HPP_ */

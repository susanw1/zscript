/*
 * UsbInternal.hpp
 *
 *  Created on: 24 Jul 2021
 *      Author: robert
 */

#ifndef LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERNAL_HPP_
#define LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERNAL_HPP_

#include <arm-no-os/llIncludes.hpp>
#include <arm-no-os/system/clock/SystemMilliClock.hpp>
#include <arm-no-os/system/interrupt/InterruptManager.hpp>
#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>
#include "UsbRegisters.hpp"

#include "UsbDefaultEndpoint.hpp"
#include "UsbInterruptDataEndpoint.hpp"

template<class LL>
class UsbInternal {
    UsbDefaultEndpoint<LL> defaultEndpoint;
    UsbInterruptDataEndpoint<LL> dataEndpoint;
    UsbRegisters *registers;
    UsbEndpointRegisters *endpointRegisters;
    UsbPbm *pbm;

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

    UsbInternal() :
            registers(NULL), endpointRegisters(NULL), pbm(NULL) {
    }

    static void UsbDataTxOverflowCallback(SerialIdentifier id) {
        (void) id;
        UsbInternal<LL>::usbI.checkBuffers();
    }

    static void UsbInterrupt(uint8_t i) {
        (void) i;
        UsbInternal<LL>::usbI.interrupt();
    }

public:
    static UsbInternal usbI;

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

        SystemMilliClock<LL>::blockDelayMillis(1);
        InterruptManager::setInterrupt(&UsbInternal<LL>::UsbInterrupt, UsbInt);

        GpioManager<LL>::activateClock(PA_11);
        GpioManager<LL>::activateClock(PA_12);
        GpioManager<LL>::getPin(PA_11).init();
        GpioManager<LL>::getPin(PA_11).setMode(Output);
        GpioManager<LL>::getPin(PA_11).setOutputSpeed(VeryHighSpeed);
        GpioManager<LL>::getPin(PA_11).setOutputMode(PushPull);
        GpioManager<LL>::getPin(PA_12).init();
        GpioManager<LL>::getPin(PA_12).setMode(Output);
        GpioManager<LL>::getPin(PA_12).setOutputSpeed(VeryHighSpeed);
        GpioManager<LL>::getPin(PA_12).setOutputMode(PushPull);

        ClockManager<LL>::getClock(HSI48)->set(48000, NONE);
        RCC->APB1ENR1 |= usbRegisterClockEnable | clockRecoveryRegisterClockEnable;
        *((uint32_t*) 0x40002000) |= clockRecoveryAutoTuneEnable | clockRecoveryEnable; //enable the CRS system for clock generation, with auto trim enabled
        RCC->CCIPR &= ~usbClockSelectMask;
        SystemMilliClock<LL>::blockDelayMillis(1);

        registers = (UsbRegisters*) 0x40005C40;
        endpointRegisters = (UsbEndpointRegisters*) 0x40005C00;
        pbm = (UsbPbm*) 0x40006000;

        registers->CNTR = transferCompleteInterruptEnable | resetInterruptEnable | usbPowerDisable;
        SystemMilliClock<LL>::blockDelayMillis(1);
        registers->CNTR = transferCompleteInterruptEnable | resetInterruptEnable;
        SystemMilliClock<LL>::blockDelayMillis(5);
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
        SystemMilliClock<LL>::blockDelayMillis(1);
        registers->CNTR |= sendResume;
        SystemMilliClock<LL>::blockDelayMillis(5);
        registers->CNTR &= ~sendResume;
        InterruptManager::enableInterrupt(UsbInt, 0, 1);
        InterruptManager::enableInterrupt(UsbInt, 1, 1);
        dataEndpoint.getTxBuffer()->setCallback(&UsbInternal<LL>::UsbDataTxOverflowCallback, LL::UsbSerialId);
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
    SerialRingBuffer<LL::UsbBufferRxSize>* getRxBuffer() {
        return dataEndpoint.getRxBuffer();
    }
    SerialRingBuffer<LL::UsbBufferTxSize>* getTxBuffer() {
        return dataEndpoint.getTxBuffer();
    }
};
template<class LL>
UsbInternal<LL> UsbInternal<LL>::usbI;

#endif /* LOWLEVEL_USBSERIALLOWLEVEL_SPECIFIC_USBINTERNAL_HPP_ */

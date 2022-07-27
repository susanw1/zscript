/*
 * AtoDManagercpp.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_ATODLOWLEVEL_SPECIFIC_ATODMANAGERCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_ATODLOWLEVEL_SPECIFIC_ATODMANAGERCPP_HPP_

#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>
#include "../AtoDManager.hpp"

template<class LL>
AtoD<LL> AtoDManager<LL>::atoDs[] = { AtoD<LL>(ADC1), AtoD<LL>(ADC2), AtoD<LL>(ADC3), AtoD<LL>(ADC4), AtoD<LL>(ADC5) };

template<class LL>
void AtoDManager<LL>::init() {
    const uint32_t enableADC345Registers = 0x4000;
    const uint32_t enableADC12Registers = 0x2000;

    const uint32_t aToD345SetSysClk = 0x80000000;
    const uint32_t aToD12SetSysClk = 0x20000000;

    RCC->AHB2ENR |= enableADC12Registers | enableADC345Registers; //enable clock to ADCs
    RCC->CCIPR |= aToD345SetSysClk | aToD12SetSysClk; //set peripheral clock inputs
    uint32_t divider = ClockManager<LL>::getClock(SysClock)->getDivider(20000 * 2);
    if (divider > 6) {
        // work out how many halvings are required to reduce the number below zero
        uint32_t newDiv = 7;
        divider >>= 3;
        while (divider > 0) {
            newDiv++;
            divider >>= 1;
        }
        divider = newDiv;
    }
    if (divider > 11) {
        divider = 11;
    }
    ADC12_COMMON->CCR |= divider << 18;
    ADC345_COMMON->CCR |= divider << 18; //setup prescalers for something reasonable
    for (int i = 0; i < LL::atoDCount; ++i) {
        atoDs[i].init();
    }
}
#define PA_0_AtoDReading() atoDs[0].performReading(1)
#define PA_1_AtoDReading() atoDs[0].performReading(2)
#define PA_2_AtoDReading() atoDs[0].performReading(3)
#define PA_3_AtoDReading() atoDs[0].performReading(4)
#define PA_4_AtoDReading() atoDs[1].performReading(17)
#define PA_5_AtoDReading() atoDs[1].performReading(13)
#define PA_6_AtoDReading() atoDs[1].performReading(3)
#define PA_7_AtoDReading() atoDs[1].performReading(4)
#define PA_8_AtoDReading() atoDs[4].performReading(1)
#define PA_9_AtoDReading() atoDs[4].performReading(2)
#define PA_10_AtoDReading() 0
#define PA_11_AtoDReading() 0
#define PA_12_AtoDReading() 0
#define PA_13_AtoDReading() 0
#define PA_14_AtoDReading() 0
#define PA_15_AtoDReading() 0

#define PB_0_AtoDReading() atoDs[2].performReading(12)
#define PB_1_AtoDReading() atoDs[0].performReading(12)
#define PB_2_AtoDReading() atoDs[1].performReading(12)
#define PB_3_AtoDReading() 0
#define PB_4_AtoDReading() 0
#define PB_5_AtoDReading() 0
#define PB_6_AtoDReading() 0
#define PB_7_AtoDReading() 0
#define PB_8_AtoDReading() 0
#define PB_9_AtoDReading() 0
#define PB_10_AtoDReading() 0
#define PB_11_AtoDReading() atoDs[0].performReading(14)
#define PB_12_AtoDReading() atoDs[0].performReading(11)
#define PB_13_AtoDReading() atoDs[2].performReading(5)
#define PB_14_AtoDReading() atoDs[0].performReading(5)
#define PB_15_AtoDReading() atoDs[3].performReading(5)

#define PC_0_AtoDReading() atoDs[0].performReading(6)
#define PC_1_AtoDReading() atoDs[0].performReading(7)
#define PC_2_AtoDReading() atoDs[0].performReading(8)
#define PC_3_AtoDReading() atoDs[0].performReading(9)
#define PC_4_AtoDReading() atoDs[1].performReading(5)
#define PC_5_AtoDReading() atoDs[1].performReading(11)
#define PC_6_AtoDReading() 0
#define PC_7_AtoDReading() 0
#define PC_8_AtoDReading() 0
#define PC_9_AtoDReading() 0
#define PC_10_AtoDReading() 0
#define PC_11_AtoDReading() 0
#define PC_12_AtoDReading() 0
#define PC_13_AtoDReading() 0
#define PC_14_AtoDReading() 0
#define PC_15_AtoDReading() 0

#define PD_0_AtoDReading() 0
#define PD_1_AtoDReading() 0
#define PD_2_AtoDReading() 0
#define PD_3_AtoDReading() 0
#define PD_4_AtoDReading() 0
#define PD_5_AtoDReading() 0
#define PD_6_AtoDReading() 0
#define PD_7_AtoDReading() 0
#define PD_8_AtoDReading() atoDs[3].performReading(12)
#define PD_9_AtoDReading() atoDs[3].performReading(13)
#define PD_10_AtoDReading() atoDs[3].performReading(7)
#define PD_11_AtoDReading() atoDs[3].performReading(8)
#define PD_12_AtoDReading() atoDs[3].performReading(9)
#define PD_13_AtoDReading() atoDs[3].performReading(10)
#define PD_14_AtoDReading() atoDs[3].performReading(11)
#define PD_15_AtoDReading() 0

#define PE_0_AtoDReading() 0
#define PE_1_AtoDReading() 0
#define PE_2_AtoDReading() 0
#define PE_3_AtoDReading() 0
#define PE_4_AtoDReading() 0
#define PE_5_AtoDReading() 0
#define PE_6_AtoDReading() 0
#define PE_7_AtoDReading() atoDs[2].performReading(4)
#define PE_8_AtoDReading() atoDs[2].performReading(6)
#define PE_9_AtoDReading() atoDs[2].performReading(2)
#define PE_10_AtoDReading() atoDs[2].performReading(14)
#define PE_11_AtoDReading() atoDs[2].performReading(15)
#define PE_12_AtoDReading() atoDs[2].performReading(16)
#define PE_13_AtoDReading() atoDs[2].performReading(3)
#define PE_14_AtoDReading() atoDs[3].performReading(1)
#define PE_15_AtoDReading() atoDs[3].performReading(2)

#define PF_0_AtoDReading() atoDs[0].performReading(10)
#define PF_1_AtoDReading() atoDs[1].performReading(10)

#define portAtoD(X) switch (pin.pin) {  \
                case Pin0:                              \
                    return P##X##_0_AtoDReading();          \
                case Pin1:                              \
                    return P##X##_1_AtoDReading();          \
                case Pin2:                              \
                    return P##X##_2_AtoDReading();          \
                case Pin3:                              \
                    return P##X##_3_AtoDReading();          \
                case Pin4:                              \
                    return P##X##_4_AtoDReading();          \
                case Pin5:                              \
                    return P##X##_5_AtoDReading();          \
                case Pin6:                              \
                    return P##X##_6_AtoDReading();          \
                case Pin7:                              \
                    return P##X##_7_AtoDReading();          \
                case Pin8:                              \
                    return P##X##_8_AtoDReading();          \
                case Pin9:                              \
                    return P##X##_9_AtoDReading();          \
                case Pin10:                              \
                    return P##X##_10_AtoDReading();          \
                case Pin11:                              \
                    return P##X##_11_AtoDReading();          \
                case Pin12:                              \
                    return P##X##_12_AtoDReading();          \
                case Pin13:                              \
                    return P##X##_13_AtoDReading();          \
                case Pin14:                              \
                    return P##X##_14_AtoDReading();          \
                case Pin15:                              \
                    return P##X##_15_AtoDReading();          \
                default:                                 \
                    return 0;                            \
                }

template<class LL>
bool AtoDManager<LL>::canPerformAtoD(GpioPinName pin) {
    if (pin.pin > 16) {
        return false;
    }
    if (pin.port == PortA) {
        if (pin.pin <= 9) {
            return true;
        }
    } else if (pin.port == PortB) {
        if (pin.pin <= 2 || pin.pin >= 11) {
            return true;
        }
    } else if (pin.port == PortC) {
        if (pin.pin <= 5) {
            return true;
        }
    } else if (pin.port == PortD) {
        if (pin.pin >= 8 && pin.pin <= 14) {
            return true;
        }
    } else if (pin.port == PortE) {
        if (pin.pin >= 7) {
            return true;
        }
    } else if (pin.port == PortF) {
        if (pin.pin == Pin0 || pin.pin == Pin1) {
            return true;
        }
    }
    return false;
}

template<class LL>
uint16_t AtoDManager<LL>::performAtoD(GpioPinName pin) {
    if (pin.port == PortA) {
        portAtoD(A);
    } else if (pin.port == PortB) {
        portAtoD(B);
    } else if (pin.port == PortC) {
        portAtoD(C);
    } else if (pin.port == PortD) {
        portAtoD(D);
    } else if (pin.port == PortE) {
        portAtoD(E);
    } else if (pin.port == PortF) {
        if (pin.pin == Pin0) {
            return PF_0_AtoDReading();
        } else if (pin.pin == Pin1) {
            return PF_1_AtoDReading();
        }
    }
    return 0;
}

#endif /* SRC_MAIN_C___LOWLEVEL_ATODLOWLEVEL_SPECIFIC_ATODMANAGERCPP_HPP_ */

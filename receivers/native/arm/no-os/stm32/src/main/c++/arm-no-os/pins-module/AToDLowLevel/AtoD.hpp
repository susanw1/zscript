/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ATODLOWLEVEL_ATOD_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ATODLOWLEVEL_ATOD_HPP_

#include <arm-no-os/llIncludes.hpp>

template<class LL>
class AtoD {
private:
    ADC_TypeDef *adcRegs;
    bool isInit = false;

public:
    AtoD(ADC_TypeDef *adcRegs) :
            adcRegs(adcRegs) {
    }

    void init();

    uint16_t performReading(uint8_t channelSource);
};

#include "specific/AtoDcpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_PINS_MODULE_ATODLOWLEVEL_ATOD_HPP_ */

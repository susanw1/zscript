/*
 * AtoD.hpp
 *
 *  Created on: 5 Jan 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATOD_HPP_
#define SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATOD_HPP_

#include "../GeneralLLSetup.hpp"

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

#endif /* SRC_TEST_C___LOWLEVEL_ATODLOWLEVEL_ATOD_HPP_ */

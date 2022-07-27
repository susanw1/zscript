/*
 * UartInternalcpp.hpp
 *
 *  Created on: 8 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNALCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNALCPP_HPP_

#include "UartInternal.hpp"
#include "../Serial.hpp"

template<class LL>
int16_t UartInternal<LL>::read() {
    const uint32_t parityError = 0x01;
    const uint32_t framingError = 0x02;
    const uint32_t noiseError = 0x04;
    const uint32_t overflowError = 0x08;
    if ((registers->ISR & parityError) != 0) {
        registers->ICR |= parityError;
        return -SerialParityError;
    } else if ((registers->ISR & framingError) != 0) {
        registers->ICR |= framingError;
        registers->RDR;
        return -SerialFramingError;
    } else if ((registers->ISR & noiseError) != 0) {
        registers->ICR |= noiseError;
        return -SerialNoiseError;
    } else if ((registers->ISR & overflowError) != 0) {
        registers->ICR |= overflowError;
        return -SerialOverflowError;
    } else {
        return registers->RDR;
    }
}

template<class LL>
bool UartInternal<LL>::hasRxFifoData() {
    const uint32_t rxFifoNotEmpty = 0x20;
    return (registers->ISR & rxFifoNotEmpty) != 0;
}

#endif /* SRC_MAIN_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTINTERNALCPP_HPP_ */

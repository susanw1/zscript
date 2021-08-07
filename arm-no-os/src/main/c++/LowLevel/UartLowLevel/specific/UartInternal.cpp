/*
 * UartInternal.cpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#include "UartInternal.hpp"

int16_t UartInternal::read() {
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
bool UartInternal::hasRxFifoData() {
    const uint32_t rxFifoNotEmpty = 0x20;
    return (registers->ISR & rxFifoNotEmpty) != 0;
}

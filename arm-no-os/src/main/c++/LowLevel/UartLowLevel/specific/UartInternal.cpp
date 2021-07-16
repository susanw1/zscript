/*
 * UartInternal.cpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#include "UartInternal.hpp"

int16_t UartInternal::read() {
    if ((registers->ISR & 0x01) != 0) {
        registers->ICR |= 0x01;
        return -UartParityError;
    } else if ((registers->ISR & 0x04) != 0) {
        registers->ICR |= 0x04;
        return -UartNoiseError;
    } else {
        return registers->RDR;
    }
}
bool UartInternal::hasRxFifoData() {
    return (registers->ISR & 0x20) != 0;
}

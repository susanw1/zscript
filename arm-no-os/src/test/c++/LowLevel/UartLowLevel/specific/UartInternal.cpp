/*
 * UartInternal.cpp
 *
 *  Created on: 12 Jul 2021
 *      Author: robert
 */

#include "UartInternal.hpp"

uint16_t UartInternal::performRead(uint8_t *buffer, uint16_t maxLength) {
    uint16_t i = 0;
    while ((registers->ISR & 0x20) && i < maxLength) {
        uint8_t value;
        if (nextValueToWrite == None) {
            value = registers->RDR;
        } else {
            value = nextValueToWrite;
            nextValueToWrite = None;
        }
        if (registers->ISR & 0x01) {
            registers->ICR |= 0x01;
            buffer[i++] = GeneralHalSetup::uartEscapingChar;
            nextValueToWrite = ParityError;

        } else if (registers->ISR & 0x04) {
            registers->ICR |= 0x04;
            buffer[i++] = GeneralHalSetup::uartEscapingChar;
            nextValueToWrite = NoiseError;

        } else {
            if (targetValueCallback != NULL && value == targetValue) {
                targetValueCallback(uart);
            }
            if (value == GeneralHalSetup::uartEscapingChar) {
                nextValueToWrite = GeneralHalSetup::uartEscapingChar;
            }
            buffer[i++] = value;
        }
    }
    return i;
}

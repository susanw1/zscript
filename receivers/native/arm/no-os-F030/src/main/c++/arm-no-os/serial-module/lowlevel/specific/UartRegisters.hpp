/*
 * UartRegisters.hpp
 *
 *  Created on: 8 Jul 2021
 *      Author: robert
 */

#ifndef SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTREGISTERS_HPP_
#define SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTREGISTERS_HPP_

#include <arm-no-os/llIncludes.hpp>

struct UartRegisters {
    volatile uint32_t CR1;
    volatile uint32_t CR2;
    volatile uint32_t CR3;
    volatile uint32_t BRR;
    volatile uint32_t Reserved;
    volatile uint32_t RTOR;
    volatile uint32_t RQR;
    volatile uint32_t ISR;
    volatile uint32_t ICR;
    volatile uint32_t RDR;
    volatile uint32_t TDR;
};

#endif /* SRC_TEST_C___LOWLEVEL_UARTLOWLEVEL_SPECIFIC_UARTREGISTERS_HPP_ */

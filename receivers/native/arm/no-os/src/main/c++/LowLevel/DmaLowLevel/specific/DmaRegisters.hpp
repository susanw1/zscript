/*
 * DmaRegisters.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMAREGISTERS_HPP_
#define SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMAREGISTERS_HPP_

#include <llIncludes.hpp>

struct DmaChannelRegisters {
    volatile uint32_t CCR;
    volatile uint32_t CNDTR;
    volatile uint32_t CPAR;
    volatile uint32_t CMAR;
    volatile uint32_t Reserved;
};

struct DmaRegisters {
    volatile uint32_t ISR;
    volatile uint32_t IFCR;
    DmaChannelRegisters CHR1;
    DmaChannelRegisters CHR2;
    DmaChannelRegisters CHR3;
    DmaChannelRegisters CHR4;
    DmaChannelRegisters CHR5;
    DmaChannelRegisters CHR6;
    DmaChannelRegisters CHR7;
    DmaChannelRegisters CHR8;
};

#endif /* SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMAREGISTERS_HPP_ */

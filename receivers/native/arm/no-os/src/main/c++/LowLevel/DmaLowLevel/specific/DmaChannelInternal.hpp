/*
 * DmaChannelInternal.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMACHANNELINTERNAL_HPP_
#define SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMACHANNELINTERNAL_HPP_

#include <LowLevel/llIncludes.hpp>
#include "DmaMuxRequest.hpp"
#include "DmaRegisters.hpp"

class DmaChannelInternal {
private:
    DmaRegisters *registers;
    DmaChannelRegisters *channelRegs;
    uint32_t *dmaMux;
    uint8_t channelOffset;

public:
    DmaChannelInternal() :
            registers(NULL), channelRegs(NULL), dmaMux(NULL), channelOffset(0) {
    }

    DmaChannelInternal(DmaRegisters *registers, DmaChannelRegisters *channelRegs, uint32_t *dmaMux, uint8_t channelOffset) :
            registers(registers), channelRegs(channelRegs), dmaMux(dmaMux), channelOffset(channelOffset) {
    }

    void setMux(DmaMuxRequest mux) {
        *dmaMux &= ~0x27F;
        *dmaMux |= mux;
    }

    void operator=(const DmaChannelInternal &i) {
        registers = i.registers;
        channelRegs = i.channelRegs;
        channelOffset = i.channelOffset;
        dmaMux = i.dmaMux;
    }

    bool hasTransferError() {
        return (8 << (channelOffset * 4)) & (registers->ISR);
    }

    void clearTransferError() {
        registers->IFCR |= 8 << (channelOffset * 4);
    }

    bool hasHalfTransferred() {
        return (4 << (channelOffset * 4)) & (registers->ISR);
    }

    void clearHalfTransferred() {
        registers->IFCR |= 4 << (channelOffset * 4);
    }

    bool hasTransferComplete() {
        return (2 << (channelOffset * 4)) & (registers->ISR);
    }

    void clearTransferComplete() {
        registers->IFCR |= 2 << (channelOffset * 4);
    }

    bool hasInterrupt() {
        return (1 << (channelOffset * 4)) & (registers->ISR);
    }

    void clearInterrupt() {
        registers->IFCR |= 1 << (channelOffset * 4);
    }

    void clearAll() {
        registers->IFCR |= 0xf << (channelOffset * 4);
    }

    DmaChannelRegisters* getChannelRegisters() {
        return channelRegs;
    }
};

#endif /* SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMACHANNELINTERNAL_HPP_ */

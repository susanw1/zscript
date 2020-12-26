/*
 * DmaChannelInternal.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMACHANNELINTERNAL_HPP_
#define SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMACHANNELINTERNAL_HPP_
#include "../../DmaLowLevel/specific/DmaRegisters.hpp"
#include "../../GeneralHalSetup.hpp"

class DmaChannelInternal {
    DmaRegisters *registers;
    DmaChannelRegisters *channelRegs;
    uint8_t channelOffset;
public:
    DmaChannelInternal() :
            registers(NULL), channelRegs(NULL), channelOffset(0) {
    }
    DmaChannelInternal(DmaRegisters *registers, DmaChannelRegisters *channelRegs, uint8_t channelOffset) :
            registers(registers), channelRegs(channelRegs), channelOffset(channelOffset) {
    }

    void operator=(const DmaChannelInternal &i) {
        registers = i.registers;
        channelRegs = i.channelRegs;
        channelOffset = i.channelOffset;
    }

    bool hasTransferError() {
        return (8 << channelOffset) & (registers->ISR);
    }

    void clearTransferError() {
        registers->IFCR |= 8 << channelOffset;
    }

    bool hasHalfTransferred() {
        return (4 << channelOffset) & (registers->ISR);
    }

    void clearHalfTransferred() {
        registers->IFCR |= 4 << channelOffset;
    }

    bool hasTransferComplete() {
        return (2 << channelOffset) & (registers->ISR);
    }

    void clearTransferComplete() {
        registers->IFCR |= 2 << channelOffset;
    }

    bool hasInterrupt() {
        return (1 << channelOffset) & (registers->ISR);
    }

    void clearInterrupt() {
        registers->IFCR |= 1 << channelOffset;
    }

    void clearAll() {
        registers->IFCR |= 0xf << channelOffset;
    }

    DmaChannelRegisters* getChannelRegisters() {
        return channelRegs;
    }
};

#endif /* SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMACHANNELINTERNAL_HPP_ */

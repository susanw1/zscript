/*
 * DmaChannelInternal.hpp
 *
 *  Created on: 18 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMACHANNELINTERNAL_HPP_
#define SRC_TEST_CPP_COMMANDS_NEWHAL_DMANEWHAL_DMACHANNELINTERNAL_HPP_

#include "DmaRegisters.hpp"
#include "../../GeneralLLSetup.hpp"
#include "DmaMuxRequest.hpp"

#ifdef __cplusplus
extern "C" {
#endif
void DMA1_Channel1_IRQHandler();
void DMA1_Channel2_IRQHandler();
void DMA1_Channel3_IRQHandler();
void DMA1_Channel4_IRQHandler();
void DMA1_Channel5_IRQHandler();
void DMA1_Channel6_IRQHandler();
void DMA1_Channel7_IRQHandler();
void DMA1_Channel8_IRQHandler();

void DMA2_Channel1_IRQHandler();
void DMA2_Channel2_IRQHandler();
void DMA2_Channel3_IRQHandler();
void DMA2_Channel4_IRQHandler();
void DMA2_Channel5_IRQHandler();
void DMA2_Channel6_IRQHandler();
void DMA2_Channel7_IRQHandler();
void DMA2_Channel8_IRQHandler();
#ifdef __cplusplus
}
#endif

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

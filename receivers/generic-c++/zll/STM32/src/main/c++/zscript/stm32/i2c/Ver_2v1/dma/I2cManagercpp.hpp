/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#define I2C_INITIALISE(x) I2c<LL>(\
        I2cInternal<LL>(I2C_SDA(I2C##x, I2C##x##_SDA), I2C_SCL(I2C##x, I2C##x##_SCL), I2C##x##_BASE),\
        x,\
        DmaRequest::DMA_RQ_I2C##x##_TX,\
        DmaRequest::DMA_RQ_I2C##x##_RX\
        )

template<class LL>
I2c<LL> I2cManager<LL>::i2cs[] = {
        ARRAY_INITIALISE_1START(I2C_INITIALISE, I2C_COUNT)
};

template<class LL>
void I2cManager<LL>::interrupt(uint8_t id) {
    i2cs[id].interrupt();
}

template<class LL>
void I2cManager<LL>::dmaInterrupt(Dma<LL> *dma, DmaTerminationStatus status) {
    for (int i = 0; i < HW::i2cCount; ++i) {
        if (I2cManager::getI2cById(i)->dma == dma) {
            I2cManager::getI2cById(i)->dmaInterrupt(status);
            break;
        }
    }
}
template<class LL>
void I2cManager<LL>::init() {
    InterruptManager::setInterrupt(&I2cManager::interrupt, InterruptType::I2cEv);
    InterruptManager::setInterrupt(&I2cManager::interrupt, InterruptType::I2cEr);
    for (int i = 0; i < HW::i2cCount; ++i) {
        i2cs[i].init();
        InterruptManager::enableInterrupt(InterruptType::I2cEv, i, 10);
        InterruptManager::enableInterrupt(InterruptType::I2cEr, i, 10);
    }
}
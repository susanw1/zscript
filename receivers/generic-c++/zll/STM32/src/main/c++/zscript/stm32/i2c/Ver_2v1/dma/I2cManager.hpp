/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

template<class LL>
class I2cManager {
    typedef typename LL::HW HW;
    private:

    friend class I2c<LL> ;
    static I2c<LL> i2cs[I2C_COUNT];

    static void interrupt(uint8_t);

#ifdef I2C_DMA
    static void dmaInterrupt(Dma<LL>*, DmaTerminationStatus);
#endif
    I2cManager() {
    }

public:
    static void init();

    static I2c<LL>* getI2cById(I2cIdentifier id) {
        return i2cs + id;
    }

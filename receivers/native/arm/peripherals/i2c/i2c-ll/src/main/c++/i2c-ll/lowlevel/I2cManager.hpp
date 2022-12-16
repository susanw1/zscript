/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_LOWLEVEL_I2CMANAGER_HPP_
#define SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_LOWLEVEL_I2CMANAGER_HPP_

#include <i2c-ll/I2cLLInterfaceInclude.hpp>
#include <llIncludes.hpp>
#include "I2c.hpp"

template<class LL>
class I2cManager {
    typedef typename LL::HW HW;
    private:

    friend class I2c<LL> ;
    static I2c<LL> i2cs[HW::i2cCount];

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
};
#include "specific/I2cManagercpp.hpp"

#endif /* SRC_MAIN_CPP_ARM_NO_OS_I2C_MODULE_LOWLEVEL_I2CMANAGER_HPP_ */

/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */


#define I2C_INITIALISE(x) I2c<LL>(\
        I2cInternal<LL>(I2C_SDA(I2C##x, I2C##x##_SDA), I2C_SCL(I2C##x, I2C##x##_SCL), I2C##x##_BASE), x)

template<class LL>
I2c<LL> I2cManager<LL>::i2cs[] = {
        ARRAY_INITIALISE_1START(I2C_INITIALISE, I2C_COUNT)
};

template<class LL>
void I2cManager<LL>::init() {
    for (int i = 0; i < HW::i2cCount; ++i) {
        i2cs[i].init();
    }
}
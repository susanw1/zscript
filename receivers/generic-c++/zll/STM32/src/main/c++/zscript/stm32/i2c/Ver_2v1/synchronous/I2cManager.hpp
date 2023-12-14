/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

template<class LL>
class I2cManager {
    typedef typename LL::HW HW;
private:

    friend class I2c<LL>;

    static I2c<LL> i2cs[I2C_COUNT];

    I2cManager() {
    }

public:
    static void init();

    static I2c<LL> *getI2cById(I2cIdentifier id) {
        return i2cs + id;
    }
};
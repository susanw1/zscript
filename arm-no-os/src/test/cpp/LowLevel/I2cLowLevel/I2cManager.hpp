/*
 * I2cManager.hpp
 *
 *  Created on: 19 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_I2CMANAGER_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_I2CMANAGER_HPP_
#include "../GeneralLLSetup.hpp"
#include "I2c.hpp"

class I2cInterruptManager;

class I2cManager {
private:
    friend I2cInterruptManager;

    static I2c i2cs[GeneralHalSetup::i2cCount];
    I2cManager() {
    }
public:
    static void init();

    static I2c* getI2cById(I2cIdentifier id) {
        return i2cs + id;
    }
};

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_I2CMANAGER_HPP_ */

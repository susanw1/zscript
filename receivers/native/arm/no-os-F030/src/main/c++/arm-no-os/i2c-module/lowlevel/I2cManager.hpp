/*
 * I2cManager.hpp
 *
 *  Created on: 19 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_I2CMANAGER_HPP_
#define SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_I2CMANAGER_HPP_

#include <arm-no-os/llIncludes.hpp>
#include "I2c.hpp"

template<class LL>
class I2cManager {
private:
    typedef typename LL::HW HW;

    friend class I2c<LL> ;
    static I2c<LL> i2cs[HW::i2cCount];

    static void interrupt(uint8_t);

    I2cManager() {
    }

public:
    static void init();

    static I2c<LL>* getI2cById(I2cIdentifier id) {
        return i2cs + id;
    }
};
#include "specific/I2cManagercpp.hpp"

#endif /* SRC_TEST_CPP_COMMANDS_LOWLEVEL_I2CLOWLEVEL_I2CMANAGER_HPP_ */

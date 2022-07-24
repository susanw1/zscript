/*
 * SPI.h
 *
 *  Created on: 30 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_
#define SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_
//FIXME: make this work without having GeneralLLSetup right there...
#include <arm-no-os/GeneralLLSetup.hpp>

#include <arm-no-os/pins-module/lowlevel/GpioManager.hpp>
#include <arm-no-os/pins-module/lowlevel/Gpio.hpp>

class SPIClass {
public:
    // Initialize the SPI library
    static void begin();

    static void beginTransaction();

    static uint8_t transfer(uint8_t data);

    static void transfer(uint8_t *buf, uint16_t count);

    static void endTransaction(void);

    static void end();
};

extern SPIClass SPI;

#endif /* SRC_TEST_CPP_LOWLEVEL_ARDUINOSPILAYER_SPI_H_ */
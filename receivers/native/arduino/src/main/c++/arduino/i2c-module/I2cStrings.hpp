/*
 * I2cStrings.hpp
 *
 *  Created on: 26 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___ARDUINO_I2C_MODULE_I2CSTRINGS_HPP_
#define SRC_MAIN_C___ARDUINO_I2C_MODULE_I2CSTRINGS_HPP_

template<class ZP>
class I2cStrings {
public:
    static const char badFreq[] PROGMEM;

    static const char noAddress[] PROGMEM;
    static const char no10BitAddress[] PROGMEM;
    static const char badAddress[] PROGMEM;

    static const char tooLong[] PROGMEM;
    static const char badPort[] PROGMEM;
    static const char tooManyRetries[] PROGMEM;

    static const char dataNack[] PROGMEM;
    static const char fatalError[] PROGMEM;
    static const char fail[] PROGMEM;
    static const char address2Nack[] PROGMEM;
    static const char addressNack[] PROGMEM;
};

template<class ZP>
const char I2cStrings<ZP>::badFreq[] PROGMEM = "Frequency field must be 0-3";

template<class ZP>
const char I2cStrings<ZP>::noAddress[] PROGMEM = "I2C address missing";
template<class ZP>
const char I2cStrings<ZP>::no10BitAddress[] PROGMEM = "10 bit I2C not supported";
template<class ZP>
const char I2cStrings<ZP>::badAddress[] PROGMEM = "I2C address out of range";

template<class ZP>
const char I2cStrings<ZP>::tooLong[] PROGMEM = "I2C requested length too large";
template<class ZP>
const char I2cStrings<ZP>::badPort[] PROGMEM = "Invalid I2C port";
template<class ZP>
const char I2cStrings<ZP>::tooManyRetries[] PROGMEM = "Retries too large";

template<class ZP>
const char I2cStrings<ZP>::dataNack[] PROGMEM = "DataNack";
template<class ZP>
const char I2cStrings<ZP>::fatalError[] PROGMEM = "Fatal I2C error";
template<class ZP>
const char I2cStrings<ZP>::fail[] PROGMEM = "I2C failure";
template<class ZP>
const char I2cStrings<ZP>::address2Nack[] PROGMEM = "Address2Nack";
template<class ZP>
const char I2cStrings<ZP>::addressNack[] PROGMEM = "AddressNack";

#endif /* SRC_MAIN_C___ARDUINO_I2C_MODULE_I2CSTRINGS_HPP_ */

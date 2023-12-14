/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */


template<class LL>
class GpioPin {
private:
    GpioPinName pin;

public:
    GpioPin(GpioPinName pin) :
            pin(pin) {
    }
    void init();

    void write(bool value);

    void set();

    void reset();

    bool read();

    void setMode(PinMode mode);

    void setOutputMode(OutputMode mode);

    void setOutputSpeed(PinSpeed speed);

    void setPullMode(PullMode mode);

    void setAlternateFunction(uint8_t function);

    GpioPinName getPinName() {
        return pin;
    }
};
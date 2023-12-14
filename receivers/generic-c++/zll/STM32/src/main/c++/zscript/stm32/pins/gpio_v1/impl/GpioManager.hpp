/*
 * Zscript Library - Command System for Microcontrollers)
 * Copyright (c) 2023 Zscript team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */


template<class LL>
class GpioManager {
private:
    typedef typename LL::HW HW;

public:
    static void activateClock(GpioPinName name);

    static GpioPin<LL> getPin(GpioPinName name);

    static GpioPort* getPort(GpioPinName name);
};

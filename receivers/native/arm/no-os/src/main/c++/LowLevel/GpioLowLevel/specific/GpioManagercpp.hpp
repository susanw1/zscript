/*
 * GpioManagercpp.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGERCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGERCPP_HPP_

#include "../GpioManager.hpp"

#define PortAAddr ((GpioPort*) 0x48000000)
#define PortBAddr ((GpioPort*) 0x48000400)
#define PortCAddr ((GpioPort*) 0x48000800)
#define PortDAddr ((GpioPort*) 0x48000C00)
#define PortEAddr ((GpioPort*) 0x48001000)
#define PortFAddr ((GpioPort*) 0x48001400)
#define PortGAddr ((GpioPort*) 0x48001800)

template<class LL>
GpioPin<LL> GpioManager<LL>::pins[] = { GpioPin<LL>( PA_0), GpioPin<LL>( PA_1), GpioPin<LL>( PA_2), GpioPin<LL>( PA_3),
        GpioPin<LL>(PortAAddr, PA_4), GpioPin<LL>(PortAAddr, PA_5), GpioPin<LL>(PortAAddr, PA_6), GpioPin<LL>( PA_7),
        GpioPin<LL>(PortAAddr, PA_8), GpioPin<LL>(PortAAddr, PA_9), GpioPin<LL>(PortAAddr, PA_10), GpioPin<LL>( PA_11),
        GpioPin<LL>(PortAAddr, PA_12), GpioPin<LL>(PortAAddr, PA_13), GpioPin<LL>(PortAAddr, PA_14), GpioPin<LL>( PA_15),

        GpioPin<LL>(PortBAddr, PB_0), GpioPin<LL>(PortBAddr, PB_1), GpioPin<LL>(PortBAddr, PB_2), GpioPin<LL>(PortBAddr, PB_3),
        GpioPin<LL>(PortBAddr, PB_4), GpioPin<LL>(PortBAddr, PB_5), GpioPin<LL>(PortBAddr, PB_6), GpioPin<LL>(PortBAddr, PB_7),
        GpioPin<LL>(PortBAddr, PB_8), GpioPin<LL>(PortBAddr, PB_9), GpioPin<LL>(PortBAddr, PB_10), GpioPin<LL>(PortBAddr, PB_11),
        GpioPin<LL>(PortBAddr, PB_12), GpioPin<LL>(PortBAddr, PB_13), GpioPin<LL>(PortBAddr, PB_14), GpioPin<LL>(PortBAddr, PB_15),

        GpioPin<LL>(PortCAddr, PC_0), GpioPin<LL>(PortCAddr, PC_1), GpioPin<LL>(PortCAddr, PC_2), GpioPin<LL>(PortCAddr, PC_3),
        GpioPin<LL>(PortCAddr, PC_4), GpioPin<LL>(PortCAddr, PC_5), GpioPin<LL>(PortCAddr, PC_6), GpioPin<LL>(PortCAddr, PC_7),
        GpioPin<LL>(PortCAddr, PC_8), GpioPin<LL>(PortCAddr, PC_9), GpioPin<LL>(PortCAddr, PC_10), GpioPin<LL>(PortCAddr, PC_11),
        GpioPin<LL>(PortCAddr, PC_12), GpioPin<LL>(PortCAddr, PC_13), GpioPin<LL>(PortCAddr, PC_14), GpioPin<LL>(PortCAddr, PC_15),

        GpioPin<LL>(PortDAddr, PD_2), GpioPin<LL>(PortFAddr, PF_0), GpioPin<LL>(PortFAddr, PF_1), GpioPin<LL>(PortGAddr, PG_10) };

template<class LL>
void GpioManager<LL>::init() {
}

template<class LL>
void GpioManager<LL>::activateClock(GpioPinName name) {
    RCC->AHB2ENR |= 1 << name.port;
}

template<class LL>
GpioPin<LL>* GpioManager<LL>::getPin(GpioPinName name) {
    //handles that the latter ports are not complete
    if (name.port <= PortC) {
        return pins + name.port * 16 + name.pin;
    } else if (name.port == PortD) {
        return pins + 48;
    } else if (name.port == PortF) {
        return pins + 49 + name.pin;
    } else {
        return pins + 51;
    }
}
template<class LL>
GpioPort* GpioManager<LL>::getPort(GpioPinName name) {
    switch (name.port) {
    case PortA:
        return PortAAddr;
    case PortB:
        return PortBAddr;
    case PortC:
        return PortCAddr;
    case PortD:
        return PortDAddr;
    case PortE:
        return PortEAddr;
    case PortF:
        return PortFAddr;
    case PortG:
        return PortGAddr;
    default:
        return NULL;
    }
}

#endif /* SRC_MAIN_C___LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGERCPP_HPP_ */

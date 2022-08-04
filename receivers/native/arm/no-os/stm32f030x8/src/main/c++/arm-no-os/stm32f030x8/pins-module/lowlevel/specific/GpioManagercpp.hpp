/*
 * GpioManagercpp.hpp
 *
 *  Created on: 6 Jul 2022
 *      Author: robert
 */

#ifndef SRC_MAIN_C___LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGERCPP_HPP_
#define SRC_MAIN_C___LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIOMANAGERCPP_HPP_

#define PortAAddr ((GpioPort*) 0x48000000)
#define PortBAddr ((GpioPort*) 0x48000400)
#define PortCAddr ((GpioPort*) 0x48000800)
#define PortDAddr ((GpioPort*) 0x48000C00)
#define PortEAddr ((GpioPort*) 0x48001000)
#define PortFAddr ((GpioPort*) 0x48001400)
#define PortGAddr ((GpioPort*) 0x48001800)

template<class LL>
GpioPin<LL> GpioManager<LL>::pins[] = { GpioPin < LL > (PA_0), GpioPin < LL > (PA_1), GpioPin < LL > (PA_2), GpioPin < LL > (PA_3),
        GpioPin < LL > (PA_4), GpioPin < LL > (PA_5), GpioPin < LL > (PA_6), GpioPin < LL > (PA_7),
        GpioPin < LL > (PA_8), GpioPin < LL > (PA_9), GpioPin < LL > (PA_10), GpioPin < LL > (PA_11),
        GpioPin < LL > (PA_12), GpioPin < LL > (PA_13), GpioPin < LL > (PA_14), GpioPin < LL > (PA_15),

        GpioPin < LL > (PB_0), GpioPin < LL > (PB_1), GpioPin < LL > (PB_2), GpioPin < LL > (PB_3),
        GpioPin < LL > (PB_4), GpioPin < LL > (PB_5), GpioPin < LL > (PB_6), GpioPin < LL > (PB_7),
        GpioPin < LL > (PB_8), GpioPin < LL > (PB_9), GpioPin < LL > (PB_10), GpioPin < LL > (PB_11),
        GpioPin < LL > (PB_12), GpioPin < LL > (PB_13), GpioPin < LL > (PB_14), GpioPin < LL > (PB_15),

        GpioPin < LL > (PC_0), GpioPin < LL > (PC_1), GpioPin < LL > (PC_2), GpioPin < LL > (PC_3),
        GpioPin < LL > (PC_4), GpioPin < LL > (PC_5), GpioPin < LL > (PC_6), GpioPin < LL > (PC_7),
        GpioPin < LL > (PC_8), GpioPin < LL > (PC_9), GpioPin < LL > (PC_10), GpioPin < LL > (PC_11),
        GpioPin < LL > (PC_12), GpioPin < LL > (PC_13), GpioPin < LL > (PC_14), GpioPin < LL > (PC_15),

        GpioPin < LL > (PD_2), GpioPin < LL > (PF_0), GpioPin < LL > (PF_1),
        GpioPin < LL > (PF_4), GpioPin < LL > (PF_5), GpioPin < LL > (PF_6), GpioPin < LL > (PF_7) };

template<class LL>
void GpioManager<LL>::init() {
}

template<class LL>
void GpioManager<LL>::activateClock(GpioPinName name) {
    RCC->AHBENR |= 1 << (name.port + 17);
}

template<class LL>
GpioPin<LL> GpioManager<LL>::getPin(GpioPinName name) {
    //handles that the latter ports are not complete
    if (name.port <= PortC) {
        return pins[name.port * 16 + name.pin];
    } else if (name.port == PortD) {
        return pins[48];
    } else if (name.port == PortF) {
        return pins[49 + name.pin];
    } else {
        return pins[51];
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

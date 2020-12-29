/*
 * GpioManager.cpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */
#include "../GpioManager.hpp"
#define PortAAddr ((GpioPort*) 0x48000000)
#define PortBAddr ((GpioPort*) 0x48000400)
#define PortCAddr ((GpioPort*) 0x48000800)
#define PortDAddr ((GpioPort*) 0x48000C00)
#define PortEAddr ((GpioPort*) 0x48001000)
#define PortFAddr ((GpioPort*) 0x48001400)
#define PortGAddr ((GpioPort*) 0x48001800)

GpioPin GpioManager::pins[] = { GpioPin(PortAAddr, PA_0), GpioPin(PortAAddr, PA_1), GpioPin(PortAAddr, PA_2), GpioPin(PortAAddr, PA_3),
        GpioPin(PortAAddr, PA_4), GpioPin(PortAAddr, PA_5), GpioPin(PortAAddr, PA_6), GpioPin(PortAAddr, PA_7),
        GpioPin(PortAAddr, PA_8), GpioPin(PortAAddr, PA_9), GpioPin(PortAAddr, PA_10), GpioPin(PortAAddr, PA_11),
        GpioPin(PortAAddr, PA_12), GpioPin(PortAAddr, PA_13), GpioPin(PortAAddr, PA_14), GpioPin(PortAAddr, PA_15),

        GpioPin(PortBAddr, PB_0), GpioPin(PortBAddr, PB_1), GpioPin(PortBAddr, PB_2), GpioPin(PortBAddr, PB_3),
        GpioPin(PortBAddr, PB_4), GpioPin(PortBAddr, PB_5), GpioPin(PortBAddr, PB_6), GpioPin(PortBAddr, PB_7),
        GpioPin(PortBAddr, PB_8), GpioPin(PortBAddr, PB_9), GpioPin(PortBAddr, PB_10), GpioPin(PortBAddr, PB_11),
        GpioPin(PortBAddr, PB_12), GpioPin(PortBAddr, PB_13), GpioPin(PortBAddr, PB_14), GpioPin(PortBAddr, PB_15),

        GpioPin(PortCAddr, PC_0), GpioPin(PortCAddr, PC_1), GpioPin(PortCAddr, PC_2), GpioPin(PortCAddr, PC_3),
        GpioPin(PortCAddr, PC_4), GpioPin(PortCAddr, PC_5), GpioPin(PortCAddr, PC_6), GpioPin(PortCAddr, PC_7),
        GpioPin(PortCAddr, PC_8), GpioPin(PortCAddr, PC_9), GpioPin(PortCAddr, PC_10), GpioPin(PortCAddr, PC_11),
        GpioPin(PortCAddr, PC_12), GpioPin(PortCAddr, PC_13), GpioPin(PortCAddr, PC_14), GpioPin(PortCAddr, PC_15),

        GpioPin(PortDAddr, PD_2), GpioPin(PortFAddr, PF_0), GpioPin(PortFAddr, PF_1), GpioPin(PortGAddr, PG_10) };

void GpioManager::init() {
}

void GpioManager::activateClock(GpioPinName name) {
    RCC->AHB2ENR |= 1 << name.port;
}

GpioPin* GpioManager::getPin(GpioPinName name) {
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

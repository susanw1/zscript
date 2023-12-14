/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

template<class LL>
void GpioManager<LL>::activateClock(GpioPinName name) {
    //FIXME clocking
    RCC->AHB2ENR |= 1 << name.port;
}

template<class LL>
GpioPin<LL> GpioManager<LL>::getPin(GpioPinName name) {
    return GpioPin<LL>(name);
}

template<class LL>
GpioPort *GpioManager<LL>::getPort(GpioPinName name) {
    switch (name.port) {
#ifdef PortA
        case PortA:
            return (GpioPort *) GPIOA_BASE;
#endif
#ifdef PortB
        case PortB:
            return (GpioPort *) GPIOB_BASE;
#endif
#ifdef PortC
        case PortC:
            return (GpioPort *) GPIOC_BASE;
#endif
#ifdef PortD
        case PortD:
            return (GpioPort *) GPIOD_BASE;
#endif
#ifdef PortE
            case PortE:
                return (GpioPort *) GPIOE_BASE;
#endif
#ifdef PortF
        case PortF:
            return (GpioPort *) GPIOF_BASE;
#endif
#ifdef PortG
        case PortG:
            return (GpioPort *) GPIOG_BASE;
#endif
#ifdef PortH
            case PortH:
                return (GpioPort *) GPIOH_BASE;
#endif
#ifdef PortI
            case PortI:
                return (GpioPort *) GPIOI_BASE;
#endif
#ifdef PortJ
            case PortJ:
            return (GpioPort *) GPIOJ_BASE;
#endif
#ifdef PortK
            case PortK:
            return (GpioPort *) GPIOK_BASE;
#endif
#ifdef PortL
            case PortL:
            return (GpioPort *) GPIOL_BASE;
#endif
            // Note that only ports known at the time of writing were included here... (with PortL as speculative)
        default:
            return NULL;
    }
}

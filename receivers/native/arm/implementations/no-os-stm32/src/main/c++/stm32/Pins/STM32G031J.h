/*
  * This file is automatically generated by pins.py from the xml descriptions provided as part of the STM32Cube IDE
*/

#ifndef LOWLEVEL_INCLUDE_DEVICE_PINS
#define LOWLEVEL_INCLUDE_DEVICE_PINS

#define PortA 0
#define PortB 1
#define PortC 2
#define PortF 3

struct GpioPinName {
    uint8_t port:4;
    uint8_t pin:4;
};


#define PA0 ((GpioPinName){PortA, Pin0})
#define PA1 ((GpioPinName){PortA, Pin1})
#define PA2 ((GpioPinName){PortA, Pin2})
#define PA8 ((GpioPinName){PortA, Pin8})
#define PA9 ((GpioPinName){PortA, Pin9})
#define PA10 ((GpioPinName){PortA, Pin10})
#define PA11 ((GpioPinName){PortA, Pin11})
#define PA12 ((GpioPinName){PortA, Pin12})
#define PA13 ((GpioPinName){PortA, Pin13})
#define PA14 ((GpioPinName){PortA, Pin14})
#define PA15 ((GpioPinName){PortA, Pin15})

#define PB0 ((GpioPinName){PortB, Pin0})
#define PB1 ((GpioPinName){PortB, Pin1})
#define PB5 ((GpioPinName){PortB, Pin5})
#define PB6 ((GpioPinName){PortB, Pin6})
#define PB7 ((GpioPinName){PortB, Pin7})
#define PB8 ((GpioPinName){PortB, Pin8})
#define PB9 ((GpioPinName){PortB, Pin9})

#define PC14 ((GpioPinName){PortC, Pin14})

#define PF2 ((GpioPinName){PortF, Pin2})

#endif /* LOWLEVEL_INCLUDE_DEVICE_PINS */
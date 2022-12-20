/*
 * This file is automatically generated by pins.py from the xml descriptions provided as part of the STM32Cube IDE
 * Timestamp: 2022-12-19T19:40:33.986195
 */

#ifndef LOWLEVEL_INCLUDE_DEVICE_PINS
#define LOWLEVEL_INCLUDE_DEVICE_PINS

#define PortA 0
#define PortB 1
#define PortC 2

struct GpioPinName {
    uint8_t port:4;
    uint8_t pin:4;
};


#define PA0 ((GpioPinName){PortA, Pin0})
#define PA1 ((GpioPinName){PortA, Pin1})
#define PA2 ((GpioPinName){PortA, Pin2})
#define PA3 ((GpioPinName){PortA, Pin3})
#define PA4 ((GpioPinName){PortA, Pin4})
#define PA5 ((GpioPinName){PortA, Pin5})
#define PA6 ((GpioPinName){PortA, Pin6})
#define PA7 ((GpioPinName){PortA, Pin7})
#define PA8 ((GpioPinName){PortA, Pin8})
#define PA9 ((GpioPinName){PortA, Pin9})
#define PA10 ((GpioPinName){PortA, Pin10})
#define PA11 ((GpioPinName){PortA, Pin11})
#define PA12 ((GpioPinName){PortA, Pin12})
#define PA13 ((GpioPinName){PortA, Pin13})
#define PA14 ((GpioPinName){PortA, Pin14})

#define PB0 ((GpioPinName){PortB, Pin0})
#define PB1 ((GpioPinName){PortB, Pin1})
#define PB4 ((GpioPinName){PortB, Pin4})
#define PB5 ((GpioPinName){PortB, Pin5})
#define PB6 ((GpioPinName){PortB, Pin6})
#define PB7 ((GpioPinName){PortB, Pin7})

#define PC14 ((GpioPinName){PortC, Pin14})
#define PC15 ((GpioPinName){PortC, Pin15})

#endif /* LOWLEVEL_INCLUDE_DEVICE_PINS */
/*
 * GpioNames.hpp
 *
 *  Created on: 23 Dec 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIONAMES_HPP_
#define SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIONAMES_HPP_

#include <LowLevel/llIncludes.hpp>

#define PortA 0
#define PortB 1
#define PortC 2
#define PortD 3
#define PortE 4
#define PortF 5
#define PortG 6

#define Pin0 0
#define Pin1 1
#define Pin2 2
#define Pin3 3
#define Pin4 4
#define Pin5 5
#define Pin6 6
#define Pin7 7
#define Pin8 8
#define Pin9 9
#define Pin10 10
#define Pin11 11
#define Pin12 12
#define Pin13 13
#define Pin14 14
#define Pin15 15

#define PA_0 ((GpioPinName){PortA, Pin0})
#define PA_1 ((GpioPinName){PortA, Pin1})
#define PA_2 ((GpioPinName){PortA, Pin2})
#define PA_3 ((GpioPinName){PortA, Pin3})
#define PA_4 ((GpioPinName){PortA, Pin4})
#define PA_5 ((GpioPinName){PortA, Pin5})
#define PA_6 ((GpioPinName){PortA, Pin6})
#define PA_7 ((GpioPinName){PortA, Pin7})
#define PA_8 ((GpioPinName){PortA, Pin8})
#define PA_9 ((GpioPinName){PortA, Pin9})
#define PA_10 ((GpioPinName){PortA, Pin10})
#define PA_11 ((GpioPinName){PortA, Pin11})
#define PA_12 ((GpioPinName){PortA, Pin12})
#define PA_13 ((GpioPinName){PortA, Pin13})
#define PA_14 ((GpioPinName){PortA, Pin14})
#define PA_15 ((GpioPinName){PortA, Pin15})

#define PB_0 ((GpioPinName){PortB, Pin0})
#define PB_1 ((GpioPinName){PortB, Pin1})
#define PB_2 ((GpioPinName){PortB, Pin2})
#define PB_3 ((GpioPinName){PortB, Pin3})
#define PB_4 ((GpioPinName){PortB, Pin4})
#define PB_5 ((GpioPinName){PortB, Pin5})
#define PB_6 ((GpioPinName){PortB, Pin6})
#define PB_7 ((GpioPinName){PortB, Pin7})
#define PB_8 ((GpioPinName){PortB, Pin8})
#define PB_9 ((GpioPinName){PortB, Pin9})
#define PB_10 ((GpioPinName){PortB, Pin10})
#define PB_11 ((GpioPinName){PortB, Pin11})
#define PB_12 ((GpioPinName){PortB, Pin12})
#define PB_13 ((GpioPinName){PortB, Pin13})
#define PB_14 ((GpioPinName){PortB, Pin14})
#define PB_15 ((GpioPinName){PortB, Pin15})

#define PC_0 ((GpioPinName){PortC, Pin0})
#define PC_1 ((GpioPinName){PortC, Pin1})
#define PC_2 ((GpioPinName){PortC, Pin2})
#define PC_3 ((GpioPinName){PortC, Pin3})
#define PC_4 ((GpioPinName){PortC, Pin4})
#define PC_5 ((GpioPinName){PortC, Pin5})
#define PC_6 ((GpioPinName){PortC, Pin6})
#define PC_7 ((GpioPinName){PortC, Pin7})
#define PC_8 ((GpioPinName){PortC, Pin8})
#define PC_9 ((GpioPinName){PortC, Pin9})
#define PC_10 ((GpioPinName){PortC, Pin10})
#define PC_11 ((GpioPinName){PortC, Pin11})
#define PC_12 ((GpioPinName){PortC, Pin12})
#define PC_13 ((GpioPinName){PortC, Pin13})
#define PC_14 ((GpioPinName){PortC, Pin14})
#define PC_15 ((GpioPinName){PortC, Pin15})

#define PD_2 ((GpioPinName){PortD, Pin2})

#define PF_0 ((GpioPinName){PortF, Pin1})
#define PF_1 ((GpioPinName){PortF, Pin2})

#define PG_10 ((GpioPinName){PortG, Pin10})

#define GPIO_AF0 0
#define GPIO_AF1 1
#define GPIO_AF2 2
#define GPIO_AF3 3
#define GPIO_AF4 4
#define GPIO_AF5 5
#define GPIO_AF6 6
#define GPIO_AF7 7
#define GPIO_AF8 8
#define GPIO_AF9 9
#define GPIO_AF10 10
#define GPIO_AF11 11
#define GPIO_AF12 12
#define GPIO_AF13 13
#define GPIO_AF14 14
#define GPIO_AF15 15

struct GpioPinName {
    uint8_t port :4;
    uint8_t pin :4;
};

#endif /* SRC_TEST_CPP_INCLUDE_LOWLEVEL_GPIOLOWLEVEL_SPECIFIC_GPIONAMES_HPP_ */

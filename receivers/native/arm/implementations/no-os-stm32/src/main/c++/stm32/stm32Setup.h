#ifndef STM32_SETUP_H
#define STM32_SETUP_H

#include "deviceSelector.h"

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

typedef uint8_t DmaIdentifier;
typedef uint8_t SerialIdentifier;
typedef uint8_t I2cIdentifier;
typedef uint8_t PinAlternateFunction;

enum PeripheralOperationMode {
    DMA, INTERRUPT, SYNCHRONOUS
};

#ifndef ZCODE_IDENTIFY_PLATFORM_HARDWARE_VERSION
#define ZCODE_IDENTIFY_PLATFORM_HARDWARE_VERSION 0x0001
#endif

#endif /* STM32_SETUP_H */

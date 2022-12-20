/*
 * This file is automatically generated by afTable.py from the xml descriptions provided as part of the STM32Cube IDE
 * Timestamp: 2022-12-19T23:52:17.404133
 */

#define FIND_VALUE_RCC_MCO_1_inner(INDEX,PIN) I2C##_RCC_MCO_##PIN
#define FIND_VALUE_RCC_MCO_1(INDEX,PIN) FIND_VALUE_RCC_MCO_1_inner(INDEX,PIN)

#define FIND_VALUE_RCC_MCO_2_inner(INDEX,PIN) I2C##_RCC_MCO_##PIN
#define FIND_VALUE_RCC_MCO_2(INDEX,PIN) FIND_VALUE_RCC_MCO_2_inner(INDEX,PIN)

#ifdef USE_RCC_MCO0
#if !FIND_VALUE_RCC_MCO_1(_RCC_MCO0, RCC_MCO0_1)
#error Invalid pin assignment for RCC_MCO0 1 pin: RCC_MCO0_1
#endif
#if !FIND_VALUE_RCC_MCO_2(_RCC_MCO0, RCC_MCO0_2)
#error Invalid pin assignment for RCC_MCO0 2 pin: RCC_MCO0_2
#endif
#endif

#ifdef USE_RCC_MCO1
#if !FIND_VALUE_RCC_MCO_1(_RCC_MCO1, RCC_MCO1_1)
#error Invalid pin assignment for RCC_MCO1 1 pin: RCC_MCO1_1
#endif
#if !FIND_VALUE_RCC_MCO_2(_RCC_MCO1, RCC_MCO1_2)
#error Invalid pin assignment for RCC_MCO1 2 pin: RCC_MCO1_2
#endif
#endif


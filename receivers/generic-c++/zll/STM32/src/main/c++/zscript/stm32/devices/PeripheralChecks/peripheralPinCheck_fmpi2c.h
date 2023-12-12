/*
 * This file is automatically generated by afTable.py from the xml descriptions provided as part of the STM32Cube IDE
 * Timestamp: 2023-12-12T18:58:56.546090
 */

#define FIND_VALUE_FMPI2C_SCL_inner(INDEX,PIN) INDEX##_FMPI2C_##PIN
#define FIND_VALUE_FMPI2C_SCL(INDEX,PIN) FIND_VALUE_FMPI2C_SCL_inner(INDEX,PIN)

#define FIND_VALUE_FMPI2C_SDA_inner(INDEX,PIN) INDEX##_FMPI2C_##PIN
#define FIND_VALUE_FMPI2C_SDA(INDEX,PIN) FIND_VALUE_FMPI2C_SDA_inner(INDEX,PIN)

#define FIND_VALUE_FMPI2C_SMBA_inner(INDEX,PIN) INDEX##_FMPI2C_##PIN
#define FIND_VALUE_FMPI2C_SMBA(INDEX,PIN) FIND_VALUE_FMPI2C_SMBA_inner(INDEX,PIN)

#ifdef USE_FMPI2C0
#if !FIND_VALUE_FMPI2C_SCL(_FMPI2C0, FMPI2C0_SCL)
#error Invalid pin assignment for FMPI2C0 SCL pin: FMPI2C0_SCL
#endif
#if !FIND_VALUE_FMPI2C_SDA(_FMPI2C0, FMPI2C0_SDA)
#error Invalid pin assignment for FMPI2C0 SDA pin: FMPI2C0_SDA
#endif
#if !FIND_VALUE_FMPI2C_SMBA(_FMPI2C0, FMPI2C0_SMBA)
#error Invalid pin assignment for FMPI2C0 SMBA pin: FMPI2C0_SMBA
#endif
#endif


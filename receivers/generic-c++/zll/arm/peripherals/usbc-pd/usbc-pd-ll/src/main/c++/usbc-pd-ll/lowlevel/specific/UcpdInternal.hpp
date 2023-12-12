/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#include <usbc-pd-ll/UsbcPdLLInterfaceInclude.hpp>

#if defined(USBC_PD_LL_STM32_Ver_ucpd_v1_0)
#include <usbc-pd-ll-stm32g/lowlevel/specific/UcpdInternal.hpp>
#else
#error Please select a supported device family
#endif

/*
 * llPinCheck.hpp
 *
 *  Created on: 19 Dec 2022
 *      Author: alicia
 */

#include <llIncludes.hpp>

#ifndef SKIP_PERIPHERAL_PIN_CHECKS

#ifdef STM32
#include "stm32/peripheralPinCheck.h"
#else
#error No supported manufacturer selected
#endif
#endif


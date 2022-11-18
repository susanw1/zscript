#if defined(ZCODE_SUPPORT_ADDRESSING) && defined(ZCODE_GENERATE_NOTIFICATIONS)
#include "zcode/modules/core/ZcodeDebugAddressingSystem.hpp"

#ifdef ZCODE_HPP_INCLUDED
#error ZcodeDebugAddressingSystem.hpp needs to be included before Zcode.hpp
#endif
#ifdef ARDUINO_USE_MODULE_ADDRESS_ROUTER
#error ZcodeDebugAddressingSystem.hpp needs to be included before an address router
#endif

#define ARDUINO_USE_MODULE_ADDRESS_ROUTER
#endif

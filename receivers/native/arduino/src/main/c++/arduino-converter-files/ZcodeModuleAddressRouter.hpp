#if defined(ZCODE_SUPPORT_ADDRESSING) && defined(ZCODE_GENERATE_NOTIFICATIONS)
#include "zcode/addressing/addressrouters/ZcodeModuleAddressRouter.hpp"

#ifdef ZCODE_HPP_INCLUDED
#error ZcodeModuleAddressRouter.hpp needs to be included before Zcode.hpp
#endif

#define ARDUINO_USE_MODULE_ADDRESS_ROUTER
#endif

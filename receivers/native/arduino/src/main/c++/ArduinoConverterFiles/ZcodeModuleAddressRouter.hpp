#if defined(ZCODE_SUPPORT_ADDRESSING) && defined(ZCODE_GENERATE_NOTIFICATIONS)
#include "zcode/addressing/ZcodeModuleAddressRouter.hpp"

ZcodeModuleAddressRouter<ZcodeParams> ZcodeModuleAddressRouterI; 

#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error ZcodeModuleAddressRouter.hpp needs to be included before Zcode.hpp
#endif

#define ARDUINO_USE_MODULE_ADDRESS_ROUTER
#endif

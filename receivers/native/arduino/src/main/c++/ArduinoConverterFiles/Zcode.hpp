#include "zcode/Zcode.hpp"


#ifdef ARDUINO_USE_MODULE_ADDRESS_ROUTER
Zcode<ZcodeParams> ZcodeI(&ZcodeModuleAddressRouterI);
#else
Zcode<ZcodeParams> ZcodeI;
#endif

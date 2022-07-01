#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#include "zcode/modules/script/ZcodeScriptModule.hpp"

ZcodeScriptModule<ZcodeParams> ZcodeScriptModuleI; 

#ifdef SRC_TEST_CPP_ZCODE_ZCODE_HPP_
#error ZcodeScriptModule.hpp needs to be included before Zcode.hpp
#endif

#define ARDUINO_USE_SCRIPT_MODULE
#endif

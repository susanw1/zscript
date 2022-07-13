//we do this to ensure we have a valid config...
#ifdef ZCODE_USE_MODULE_ADDRESSING
#define ZCODE_SUPPORT_ADDRESSING
#endif
#ifdef ZCODE_SUPPORT_INTERRUPT_VECTOR
#define ZCODE_SUPPORT_SCRIPT_SPACE
#endif
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#define ZCODE_GENERATE_NOTIFICATIONS
#endif
#ifdef ZCODE_SUPPORT_ADDRESSING
#define ZCODE_GENERATE_NOTIFICATIONS
#endif

#include "ZcodeScriptModule.hpp"
#include "ZcodeCoreModule.hpp"
#include "ZcodeOuterCoreModule.hpp"

#include "ZcodeModuleAddressRouter.hpp"

#include "Zcode.hpp"
#include "ZcodeSerialChannel.hpp"

class ArduinoZcodeBasicSetup {
#ifdef ZCODE_HAVE_SERIAL_CHANNEL
		ZcodeCommandChannel<ZcodeParams> *channels[1];
#endif
		static const uint8_t moduleCount = 2
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
		+1
#endif
		;

		ZcodeModule<ZcodeParams> *modules[moduleCount];
public:

	void setup(){
#ifdef ARDUINO_USE_MODULE_ADDRESS_ROUTER
		Zcode<ZcodeParams>::zcode.setAddressRouter(&ZcodeModuleAddressRouterI);
#endif
#ifdef ZCODE_HAVE_SERIAL_CHANNEL
		channels[0] = &ZcodeSerialChannelI;
		Zcode<ZcodeParams>::zcode.setChannels(channels, 1);
#endif
		uint8_t i = 0;
		modules[i++] = &ZcodeCoreModuleI;
		modules[i++] = &ZcodeOuterCoreModuleI;
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
		modules[i++] = &ZcodeScriptModuleI;
#endif
		Zcode<ZcodeParams>::zcode.setModules(modules, moduleCount);
	}
};
ArduinoZcodeBasicSetup ZcodeSetup;

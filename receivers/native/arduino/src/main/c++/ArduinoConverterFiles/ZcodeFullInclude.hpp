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

#include "ZcodeCoreModule.hpp"
#include "ZcodeScriptModule.hpp"

#include "ZcodeModuleAddressRouter.hpp"

#include "Zcode.hpp"
#include "ZcodeSerialChannel.hpp"

class ArduinoZcodeBasicSetup {
#ifdef ZCODE_HAVE_SERIAL_CHANNEL
		ZcodeCommandChannel<ZcodeParams> *channels[1];
#endif
		static const uint8_t moduleCount = 1
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
		+1
#endif
		;

		ZcodeModule<ZcodeParams> *modules[moduleCount];
public:

	void setup(){
#ifdef ZCODE_HAVE_SERIAL_CHANNEL
		channels[0] = &ZcodeSerialChannelI;
		ZcodeI.setChannels(channels, 1);
#endif
		uint8_t i = 0;
		modules[i++] = &ZcodeCoreModuleI;
#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
		modules[i++] = &ZcodeScriptModuleI;
#endif
		ZcodeI.setModules(modules, moduleCount);
	}
};
ArduinoZcodeBasicSetup ZcodeSetup;

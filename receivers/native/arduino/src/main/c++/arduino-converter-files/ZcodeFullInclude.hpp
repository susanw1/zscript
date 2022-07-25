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

#ifdef ZCODE_USE_DEBUG_ADDRESSING_SYSTEM
#include "ZcodeDebugAddressingSystem.hpp"
#define ZCODE_SUPPORT_DEBUG
#endif
#ifdef ZCODE_USE_MODULE_ADDRESSING
#include "ZcodeModuleAddressRouter.hpp"
#endif
#ifdef ZCODE_USE_MAPPING_ADDRESSING
#include "ZcodeMappingAddressRouter.hpp"
#endif

// makes the above #define's the only needed ones...
#ifdef ARDUINO_USE_MODULE_ADDRESS_ROUTER
#define ZCODE_SUPPORT_ADDRESSING
#endif

#ifdef ZCODE_SUPPORT_ADDRESSING
#define ZCODE_GENERATE_NOTIFICATIONS
#endif

#include "ZcodeScriptModule.hpp"
#include "ZcodeOuterCoreModule.hpp"
#include "ZcodeCoreModule.hpp"

#include "Zcode.hpp"

#ifdef ZCODE_HAVE_SERIAL_CHANNEL
#include "arduino/serial-module/channels/ZcodeSerialChannel.hpp"

ZcodeSerialChannel<ZcodeParams> ZcodeSerialChannelI;
#endif

class ArduinoZcodeBasicSetup {
#ifdef ZCODE_HAVE_SERIAL_CHANNEL
		ZcodeCommandChannel<ZcodeParams> *channels[1];
#endif

public:

    void setup() {

#ifdef ZCODE_HAVE_SERIAL_CHANNEL
		channels[0] = &ZcodeSerialChannelI;
		Zcode<ZcodeParams>::zcode.setChannels(channels, 1);
#endif
    }
};
ArduinoZcodeBasicSetup ZcodeSetup;

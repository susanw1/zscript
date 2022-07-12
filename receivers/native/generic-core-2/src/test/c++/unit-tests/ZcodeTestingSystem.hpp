#include "../support/ZcodeParameters.hpp"

#include "modules/script/ZcodeScriptModule.hpp"
#include "modules/outer-core/ZcodeOuterCoreModule.hpp"
#include "modules/core/ZcodeCoreModule.hpp"

#include "addressing/ZcodeModuleAddressRouter.hpp"
#include "../support/ZcodeTestChannel.hpp"

#include "Zcode.hpp"

class ZcodeTestingSystem {
public:
    static bool performTest(const char *input, uint8_t *output, uint16_t outLength) {

        ZcodeModuleAddressRouter<TestParams> addrRouter;
        Zcode<TestParams> zcode(&addrRouter);

        ZcodeTestChannel localChannel(&zcode, input, output, outLength);
        ZcodeCommandChannel<TestParams> *channels[1] = { &localChannel };
        zcode.setChannels(channels, 1);
        ZcodeCoreModule<TestParams> core;
        ZcodeOuterCoreModule<TestParams> outerCore;
        ZcodeScriptModule<TestParams> script;
        ZcodeModule<TestParams> *modules[3] = { &core, &outerCore, &script };
        zcode.setModules(modules, 3);

        while (!localChannel.isDone()) {
            zcode.progressZcode();
        }
        return localChannel.isLengthExceeded();
    }
    static bool tryTest(const char *input, const char *expectedOut) {
        uint16_t outLen = 0;
        while (expectedOut[outLen++] != 0)
            ;
        outLen--;
        uint8_t output[outLen];
        bool exceeded = performTest(input, output, outLen);
        if (exceeded) {
            std::cerr << "Response too long\n"
                    << "Wanted: " << expectedOut
                    << "\nGot: ";
            std::cerr.write((char*) output, outLen);
            std::cerr << "\n";
            return false;
        }
        for (int i = 0; i < outLen; ++i) {
            if (output[i] != expectedOut[i]) {
                std::cerr << "Response didn't match\n"
                        << "Wanted: " << expectedOut
                        << "\nGot: ";
                std::cerr.write((char*) output, outLen + 20);
                std::cerr << "\n";
                return false;
            }
        }
        return true;
    }

};

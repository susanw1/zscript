#include "../support/ZcodeParameters.hpp"

#ifdef ZCODE_SUPPORT_ADDRESSING
#ifdef ZCODE_SUPPORT_DEBUG
#include <zcode/modules/core/ZcodeDebugAddressingSystem.hpp>
#endif

#include <zcode/addressing/addressrouters/ZcodeModuleAddressRouter.hpp>
#endif

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#include <zcode/modules/script/ZcodeScriptModule.hpp>
#endif

#include <zcode/modules/outer-core/ZcodeOuterCoreModule.hpp>
#include <zcode/modules/core/ZcodeCoreModule.hpp>

#include "../support/ZcodeTestChannel.hpp"

#include <zcode/Zcode.hpp>

class ZcodeTestingSystem {
public:
    static bool performTest(const char *input, uint8_t *output, uint16_t outLength) {
        Zcode<TestParams> tmpZ;
        Zcode<TestParams>::zcode = tmpZ;
        Zcode<TestParams> *zcode = &Zcode<TestParams>::zcode;
        ZcodeTestChannel localChannel(input, output, outLength);
        ZcodeCommandChannel<TestParams> *channels[1] = { &localChannel };
        zcode->setChannels(channels, 1);

        while (!localChannel.isDone()) {
            zcode->progressZcode();
        }
        return localChannel.isLengthExceeded();
    }
    static bool tryTest(const char *input, const char *expectedOut) {
        uint16_t outLen = 0;
        while (expectedOut[outLen++] != 0)
            ;
        outLen--;
        uint8_t output[outLen + 200];
        bool exceeded = performTest(input, output, outLen);
        if (exceeded) {
            std::cerr << "Response too long\n" << outLen << '\n'
                    << "Wanted: " << expectedOut
                    << "\nGot: ";
            std::cerr.write((char*) output, outLen + 200);
            std::cerr << "\n";
            return false;
        }
        for (int i = 0; i < outLen; ++i) {
            if (output[i] != expectedOut[i]) {
                std::cerr << "Response didn't match\n"
                        << "Wanted: " << expectedOut
                        << "\nGot: ";
                std::cerr.write((char*) output, outLen);
                std::cerr << "\n";
                return false;
            }
        }
        return true;
    }

};

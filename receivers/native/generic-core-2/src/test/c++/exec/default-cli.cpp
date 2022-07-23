/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include <iostream>

#include "../support/ZcodeParameters.hpp"

#ifdef ZCODE_SUPPORT_ADDRESSING
#ifdef ZCODE_SUPPORT_DEBUG
#include "modules/core/ZcodeDebugAddressingSystem.hpp"
#endif

#include <addressing/addressrouters/ZcodeModuleAddressRouter.hpp>
#endif

#ifdef ZCODE_SUPPORT_SCRIPT_SPACE
#include "modules/script/ZcodeScriptModule.hpp"
#endif

#include "modules/outer-core/ZcodeOuterCoreModule.hpp"
#include "modules/core/ZcodeCoreModule.hpp"

#include "../support/ZcodeLocalChannel.hpp"

#include "Zcode.hpp"

int main(void) {
    std::cout << "size of Zcode: " << sizeof(Zcode<TestParams> ) << "\n";

    Zcode<TestParams> *zcode = &Zcode<TestParams>::zcode;
    ZcodeLocalChannel localChannel;
    ZcodeCommandChannel<TestParams> *channels[1] = { &localChannel };
    zcode->setChannels(channels, 1);
    while (true) {
        zcode->progressZcode();
    }
    return 0;
}

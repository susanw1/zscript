/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include <iostream>

#include "../support/ZcodeParameters.hpp"

#include "modules/core/ZcodeDebugAddressingSystem.hpp"
#include <addressing/addressrouters/ZcodeModuleAddressRouter.hpp>
//#include "modules/script/ZcodeScriptModule.hpp"
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

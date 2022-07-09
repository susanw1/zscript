/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include <iostream>

#include "../support/ZcodeParameters.hpp"

//#include "modules/script/ZcodeScriptModule.hpp"
#include "modules/core/ZcodeCoreModule.hpp"

#include "addressing/ZcodeModuleAddressRouter.hpp"
#include "../support/ZcodeLocalChannel.hpp"

#include "Zcode.hpp"

int main(void) {
    std::cout << "size of Zcode: " << sizeof(Zcode<TestParams> ) << "\n";

    ZcodeModuleAddressRouter<TestParams> addrRouter;
    Zcode<TestParams> zcode(&addrRouter);
    ZcodeLocalChannel localChannel(&zcode);
    ZcodeCommandChannel<TestParams> *channels[1] = { &localChannel };
    zcode.setChannels(channels, 1);
    ZcodeCoreModule<TestParams> core;
//    ZcodeScriptModule<TestParams> script;
    ZcodeModule<TestParams> *modules[1] = { &core /*, &script*/};
    zcode.setModules(modules, 1);

    while (true) {
        zcode.progressZcode();
    }
    return 0;
}
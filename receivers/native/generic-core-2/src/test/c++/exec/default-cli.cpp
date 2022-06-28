/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "modules/core/ZcodeCoreModule.hpp"
#include "parsing/ZcodeChannelCommandSlot.hpp"

#include "../support/ZcodeLocalChannel.hpp"

#include <iostream>

#include "scriptspace/ZcodeScriptSpaceChannel.hpp"
#include "modules/ZcodeModule.hpp"

#include "Zcode.hpp"
int main(void) {
    std::cout << "size of Zcode: " << sizeof(Zcode<TestParams> ) << "\n";
//    std::cout << "size of ZcodeCommandSlot: " << sizeof(ZcodeCommandSlot<TestParams> ) << "\n";

    Zcode<TestParams> zcode(NULL, 0);

    ZcodeLocalChannel localChannel(&zcode);
    ZcodeScriptSpaceChannel<TestParams> execSpaceChannel(&zcode);
    ZcodeCommandChannel<TestParams> *channels[2] = { &localChannel, &execSpaceChannel };
    ZcodeScriptSpaceChannel<TestParams> **execChannelPtr = (ZcodeScriptSpaceChannel<TestParams>**) channels + 1;
    zcode.setChannels(channels, 2);
    zcode.getSpace()->setChannels(execChannelPtr, 1);
    ZcodeCoreModule<TestParams> core;
    ZcodeModule<TestParams> *modules[1] = { &core };
    zcode.setModules(modules, 1);

    while (true) {
        zcode.progressZcode();
    }
    return 0;
}

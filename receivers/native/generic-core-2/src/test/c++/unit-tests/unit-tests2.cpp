/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "Zcode.hpp"

#include "commands/ZcodeEchoCommand.hpp"
#include "commands/ZcodeCapabilitiesCommand.hpp"
#include "commands/ZcodeActivateCommand.hpp"
#include "commands/ZcodeSetDebugChannelCommand.hpp"
#include "commands/ZcodeSendDebugCommand.hpp"
#include "commands/ZcodeExecutionStateCommand.hpp"
#include "commands/ZcodeExecutionCommand.hpp"
#include "commands/ZcodeExecutionStoreCommand.hpp"
#include "commands/ZcodeNotificationHostCommand.hpp"
#include "../support/ZcodeLocalChannel.hpp"

#include <iostream>

#include "../../../main/c++/zcode/scriptspace/ZcodeScriptSpaceChannel.hpp"

int main(void) {
    std::cout << ">>> unit-tests2 executing! <<<" << std::endl;
    Zcode<TestParams> zcode(NULL, 0);

    ZcodeLocalChannel localChannel(&zcode);
    ZcodeScriptSpaceChannel<TestParams> execSpaceChannel(&zcode, zcode.getSpace());
    ZcodeCommandChannel<TestParams> *channels[2] = { &localChannel, &execSpaceChannel };
    ZcodeScriptSpaceChannel<TestParams> **execChannelPtr = (ZcodeScriptSpaceChannel<TestParams>**) channels + 1;
    zcode.setChannels(channels, 2);
    zcode.getSpace()->setChannels(execChannelPtr, 1);

    ZcodeEchoCommand<TestParams> cmd0;
    ZcodeActivateCommand<TestParams> cmd1;
    ZcodeSetDebugChannelCommand<TestParams> cmd2(&zcode);
    ZcodeSendDebugCommand<TestParams> cmd3(&zcode);
    ZcodeExecutionStateCommand<TestParams> cmd4(zcode.getSpace());
    ZcodeExecutionCommand<TestParams> cmd5(zcode.getSpace());
    ZcodeExecutionStoreCommand<TestParams> cmd6(zcode.getSpace());
    ZcodeNotificationHostCommand<TestParams> cmd7(&zcode);
    ZcodeCapabilitiesCommand<TestParams> cmd8(&zcode);

    zcode.getCommandFinder()->registerCommand(&cmd0)->registerCommand(&cmd1)->registerCommand(&cmd2)->registerCommand(&cmd3)->registerCommand(&cmd4)->registerCommand(
            &cmd5)->registerCommand(&cmd6)->registerCommand(&cmd7)->registerCommand(&cmd8);
//    while (!std::cin.eof()) {
//        zcode.progressZcode();
//    }
    return 0;
}

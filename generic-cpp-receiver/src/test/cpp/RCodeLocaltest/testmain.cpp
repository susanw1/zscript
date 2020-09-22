/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */
#include "../RCode/RCode.hpp"
#include "RCodeLocalChannel.hpp"
#include "../RCode/commands/RCodeEchoCommand.hpp"
#include "../RCode/commands/RCodeActivateCommand.hpp"
#include "../RCode/commands/RCodeSetDebugChannelCommand.hpp"
#include "../RCode/commands/RCodeSendDebugCommand.hpp"
#include <iostream>

int main(void) {
    std::cout << "size of RCode: " << sizeof(RCode) << "\n";
    std::cout << "size of RCodeCommandSlot: " << sizeof(RCodeCommandSlot)
            << "\n";
    RCode r = RCode();
    RCodeLocalChannel ch = RCodeLocalChannel(&r);
    RCodeCommandChannel *chptr = &ch;
    r.setChannels(&chptr, 1);
    RCodeEchoCommand cmd0 = RCodeEchoCommand();
    RCodeActivateCommand cmd1 = RCodeActivateCommand();
    RCodeSetDebugChannelCommand cmd2 = RCodeSetDebugChannelCommand(&r);
    RCodeSendDebugCommand cmd3 = RCodeSendDebugCommand(&r);
    r.getCommandFinder()->registerCommand(&cmd0);
    r.getCommandFinder()->registerCommand(&cmd1);
    r.getCommandFinder()->registerCommand(&cmd2);
    r.getCommandFinder()->registerCommand(&cmd3);
    while (true) {
        r.progressRCode();
    }
}

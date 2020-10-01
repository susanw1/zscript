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
#include "../RCode/commands/RCodeExecutionStateCommand.hpp"
#include "../RCode/commands/RCodeExecutionCommand.hpp"
#include "../RCode/commands/RCodeExecutionStoreCommand.hpp"
#include "../RCode/commands/RCodeNotificationHostCommand.hpp"
#include "../RCode/executionspace/RCodeExecutionSpaceChannel.hpp"
#include <iostream>

int main(void) {
    std::cout << "size of RCode: " << sizeof(RCode) << "\n";
    std::cout << "size of RCodeCommandSlot: " << sizeof(RCodeCommandSlot)
            << "\n";
    RCode r = RCode(NULL, 0);
    RCodeLocalChannel ch = RCodeLocalChannel(&r);
    RCodeExecutionSpaceChannel execCh = RCodeExecutionSpaceChannel(&r,
            r.getSpace());
    RCodeCommandChannel *chptr[2] = { &execCh, &ch };
    RCodeExecutionSpaceChannel **execPtr = (RCodeExecutionSpaceChannel**) chptr
            + 1;
    r.setChannels(chptr, 2);
    r.getSpace()->setChannels(execPtr, 1);
    RCodeEchoCommand cmd0 = RCodeEchoCommand();
    RCodeActivateCommand cmd1 = RCodeActivateCommand();
    RCodeSetDebugChannelCommand cmd2 = RCodeSetDebugChannelCommand(&r);
    RCodeSendDebugCommand cmd3 = RCodeSendDebugCommand(&r);
    RCodeExecutionStateCommand cmd4 = RCodeExecutionStateCommand(r.getSpace());
    RCodeExecutionCommand cmd5 = RCodeExecutionCommand(r.getSpace());
    RCodeExecutionStoreCommand cmd6 = RCodeExecutionStoreCommand(r.getSpace());
    RCodeNotificationHostCommand cmd7 = RCodeNotificationHostCommand(&r);
    r.getCommandFinder()->registerCommand(&cmd0);
    r.getCommandFinder()->registerCommand(&cmd1);
    r.getCommandFinder()->registerCommand(&cmd2);
    r.getCommandFinder()->registerCommand(&cmd3);
    r.getCommandFinder()->registerCommand(&cmd4);
    r.getCommandFinder()->registerCommand(&cmd5);
    r.getCommandFinder()->registerCommand(&cmd6);
    r.getCommandFinder()->registerCommand(&cmd7);
    while (true) {
        r.progressRCode();
    }
}

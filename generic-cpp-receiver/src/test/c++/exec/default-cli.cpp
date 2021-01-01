/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCode.hpp"

#include "commands/RCodeEchoCommand.hpp"
#include "commands/RCodeCapabilitiesCommand.hpp"
#include "commands/RCodeActivateCommand.hpp"
#include "commands/RCodeSetDebugChannelCommand.hpp"
#include "commands/RCodeSendDebugCommand.hpp"
#include "commands/RCodeExecutionStateCommand.hpp"
#include "commands/RCodeExecutionCommand.hpp"
#include "commands/RCodeExecutionStoreCommand.hpp"
#include "commands/RCodeNotificationHostCommand.hpp"
#include "executionspace/RCodeExecutionSpaceChannel.hpp"

#include "../support/RCodeLocalChannel.hpp"

#include <iostream>

int main(void) {
    std::cout << "size of RCode: " << sizeof(RCode<TestParams> ) << "\n";
    std::cout << "size of RCodeCommandSlot: " << sizeof(RCodeCommandSlot<TestParams> ) << "\n";

    RCode<TestParams> rcode(NULL, 0);

    RCodeLocalChannel localChannel(&rcode);
    RCodeExecutionSpaceChannel<TestParams> execSpaceChannel(&rcode, rcode.getSpace());
    RCodeCommandChannel<TestParams> *channels[2] = { &localChannel, &execSpaceChannel };
    RCodeExecutionSpaceChannel<TestParams> **execChannelPtr = (RCodeExecutionSpaceChannel<TestParams>**) channels + 1;
    rcode.setChannels(channels, 2);
    rcode.getSpace()->setChannels(execChannelPtr, 1);

    RCodeEchoCommand<TestParams> cmd0;
    RCodeActivateCommand<TestParams> cmd1;
    RCodeSetDebugChannelCommand<TestParams> cmd2(&rcode);
    RCodeSendDebugCommand<TestParams> cmd3(&rcode);
    RCodeExecutionStateCommand<TestParams> cmd4(rcode.getSpace());
    RCodeExecutionCommand<TestParams> cmd5(rcode.getSpace());
    RCodeExecutionStoreCommand<TestParams> cmd6(rcode.getSpace());
    RCodeNotificationHostCommand<TestParams> cmd7(&rcode);
    RCodeCapabilitiesCommand<TestParams> cmd8(&rcode);

    rcode.getCommandFinder()
            ->registerCommand(&cmd0)
            ->registerCommand(&cmd1)
            ->registerCommand(&cmd2)
            ->registerCommand(&cmd3)
            ->registerCommand(&cmd4)
            ->registerCommand(&cmd5)
            ->registerCommand(&cmd6)
            ->registerCommand(&cmd7)
            ->registerCommand(&cmd8);

    while (true) {
        rcode.progressRCode();
    }
    return 0;
}

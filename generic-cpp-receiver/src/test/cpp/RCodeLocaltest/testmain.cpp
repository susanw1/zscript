/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */
#include "../RCode/RCode.hpp"
#include "RCodeLocalChannel.hpp"
#include "../RCode/commands/RCodeEchoCommand.hpp"
#include <iostream>

int main(void) {
    std::cout << "size of RCode: " << sizeof(RCode) << "\n";
    std::cout << "size of RCodeCommandSlot: " << sizeof(RCodeCommandSlot)
            << "\n";
    RCode r = RCode();
    RCodeLocalChannel ch = RCodeLocalChannel(&r);
    RCodeCommandChannel *chptr = &ch;
    r.setChannels(&chptr, 1);
    RCodeEchoCommand cmd = RCodeEchoCommand();
    r.getCommandFinder()->registerCommand(&cmd);
    while (true) {
        r.progressRCode();
    }
}

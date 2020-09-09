/*
 * testmain.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */
#include "../RCode.hpp"
#include "RCodeLocalChannel.hpp"
#include "../commands/RCodeEchoCommand.hpp"

int main(void) {
    RCode r = RCode();
    RCodeLocalChannel ch = RCodeLocalChannel(&r);
    r.setChannels(&ch, 1);
    RCodeEchoCommand cmd = RCodeEchoCommand();
    r.getCommandFinder()->registerCommand(&cmd);
    while (true) {
        r.progressRCode();
    }
}

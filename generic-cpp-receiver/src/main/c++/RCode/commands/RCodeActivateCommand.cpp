/*
 * RCodeActivateCommand.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeActivateCommand.hpp"

bool RCodeActivateCommand::activated = false;

void RCodeActivateCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    if (!activated) {
        out->writeField('A', 0);
        out->writeStatus(OK);
    } else {
        out->writeField('A', 1);
        out->writeStatus(OK);
    }
    activated = true;
    slot->setComplete(true);
}


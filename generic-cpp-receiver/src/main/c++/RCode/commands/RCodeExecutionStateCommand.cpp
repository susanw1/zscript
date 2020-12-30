/*
 * RCodeExecutionStateCommand.cpp
 *
 *  Created on: 29 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionStateCommand.hpp"

#ifdef NOTIFICATIONS
void RCodeExecutionStateCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    out->writeField('D', space->getDelay());
    if (space->hasFailed()) {
        out->writeField('F', 0);
    }
    out->writeField('G', space->isRunning());
    RCodeOutStream::writeFieldType(out, 'L', space->getLength());
    RCodeOutStream::writeFieldType(out, 'M', RCodeParameters::executionLength);
    out->writeStatus(OK);
    slot->setComplete(true);
}
#endif

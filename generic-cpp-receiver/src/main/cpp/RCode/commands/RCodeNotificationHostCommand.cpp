/*
 * RCodeNotificationHostCommand.cpp
 *
 *  Created on: 30 Sep 2020
 *      Author: robert
 */

#include "RCodeNotificationHostCommand.hpp"

#ifdef NOTIFICATIONS
void RCodeNotificationHostCommand::execute(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence, RCodeOutStream *out) {
    rcode->getNotificationManager()->setNotificationChannel(
            sequence->getChannel());
    out->writeStatus(OK);
    slot->setComplete(true);
}

#endif

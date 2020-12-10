/*
 * RCodeExecutionSpaceOut.cpp
 *
 *  Created on: 28 Sep 2020
 *      Author: robert
 */

#include "RCodeExecutionSpaceOut.hpp"

bool RCodeExecutionSpaceOut::flush() {
    if (status != CMD_FAIL && status != OK) {
        space->setRunning(false);
        space->setFailed(true);
    }
    if (!space->getNotificationChannel()->hasOutStream()
            || !space->getNotificationChannel()->acquireOutStream()->isLocked()) {
        RCodeOutStream *out =
                space->getNotificationChannel()->acquireOutStream();
        out->lock();
        if (out->isOpen()) {
            out->close();
        }
        out->openNotification(space->getNotificationChannel());
        out->mostRecent = space;
        out->markNotification();
        out->writeField('Z', 2);
        if (overLength) {
            out->writeBytes(buffer, lastEndPos);
            out->writeCommandSeperator();
            out->writeStatus(RESP_TOO_LONG);
            out->writeCommandSeperator();
            out->writeStatus(status);
            out->writeCommandSequenceSeperator();
        } else {
            out->writeBytes(buffer, length);
        }
        out->close();
        out->unlock();
        space->getNotificationChannel()->releaseOutStream();
        dataBufferFull = false;
        status = OK;
        return true;
    } else {
        dataBufferFull = true;
        status = OK;
        return false;
    }
}

/*
 * RCodeRunner.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeRunner.hpp"

void RCodeRunner::runNext() {
    RCodeCommandSequence *current = NULL;
    int targetInd = 0;
    for (; targetInd < parallelNum; targetInd++) {
        if (running[targetInd]->peekFirst()->isComplete()) {
            current = running[targetInd];
            break;
        }
    }
    if (current != NULL) {
        if (finishRunning(current, targetInd)) {
            current = NULL;
        }
    }
    if (current == NULL && !canBeParallel && parallelNum == 1
            && running[0]->canBeParallel() && running[0]->isFullyParsed()
            && running[0]->canLock()) {
        running[0]->lock();
        canBeParallel = true;
    }
    if (current == NULL && parallelNum < RCodeParameters::maxParallelRunning) {
        RCodeCommandChannel **channels = rcode->getChannels();
        if (canBeParallel || parallelNum == 0) {
            canBeParallel = true;
            for (int i = 0; i < rcode->getChannelNumber(); i++) {
                if (channels[i]->getCommandSequence()->isFullyParsed()
                        && !channels[i]->getOutStream()->isLocked()
                        && channels[i]->getCommandSequence()->canLock()
                        && channels[i]->getCommandSequence()->canBeParallel()) {
                    current = channels[i]->getCommandSequence();
                    targetInd = i;
                    current->lock();
                    break;
                }
            }
        }
        if (canBeParallel && current == NULL && parallelNum == 0) {
            for (int i = 0; i < rcode->getChannelNumber(); i++) {
                if (channels[i]->getCommandSequence()->peekFirst() != NULL
                        && !channels[i]->getOutStream()->isLocked()) {
                    current = channels[i]->getCommandSequence();
                    targetInd = i;
                    canBeParallel = false;
                    break;
                }
            }
        }
        if (current != NULL) {
            RCodeOutStream *out = current->getOutStream();
            if (out->isOpen() && out->mostRecent != current) {
                out->close();
            }
            out->mostRecent = current;
            if (!out->isOpen()) {
                out->openResponse(current->getChannel());
            }
            if (current->isBroadcast()) {
                out->markBroadcast();
            }
            running[parallelNum++] = current;
        }
    }
    if (current != NULL) {
        runSequence(current, targetInd);
    }
}
bool RCodeRunner::finishRunning(RCodeCommandSequence *target, int targetInd) {
    if (target->peekFirst()->isStarted()) {
        target->peekFirst()->getCommand(rcode)->finish(target->peekFirst(),
                target->getOutStream());
    }
    if (target->peekFirst()->getEnd() == '\n') {
        target->getOutStream()->writeCommandSequenceSeperator();
    } else {
        target->getOutStream()->writeCommandSeperator();
    }
    RCodeCommandSlot *slot = target->popFirst();
    slot->reset();
    if (target->peekFirst() == NULL) {
        if (!target->getChannel()->isPacketBased()
                || (target->isFullyParsed()
                        && !target->getChannel()->hasCommandSequence())) {
            target->getOutStream()->close();
        }
        target->releaseOutStream();
        if (!target->canBeParallel()) {
            canBeParallel = true;
        } else if (parallelNum == 1) {
            canBeParallel = true;
        }
        target->unlock();
        target->reset();
        for (int i = targetInd; i < parallelNum - 1; i++) {
            running[i] = running[i + 1];
        }
        parallelNum--;
        return true;
    }
    return false;
}

void RCodeRunner::runSequence(RCodeCommandSequence *target, int targetInd) {
    RCodeOutStream *out = target->getOutStream();
    RCodeCommandSlot *cmd = target->peekFirst();
    cmd->getFields()->copyFieldTo(out, 'E');
    if (cmd->getStatus() != OK) {
        cmd->setComplete(true);
        out->writeStatus(cmd->getStatus());
        out->writeBigStringField(cmd->getErrorMessage());
    } else {
        RCodeCommand *c = cmd->getCommand(rcode);
        if (c == NULL) {
            out->writeStatus(UNKNOWN_CMD);
            out->writeBigStringField("Command not found");
            target->fail();
            finishRunning(target, targetInd);
        } else if (cmd->getFields()->get('R', 0xFF)
                > RCodeActivateCommand::MAX_SYSTEM_CODE
                && !RCodeActivateCommand::isActivated()) {
            out->writeStatus(NOT_ACTIVATED);
            out->writeBigStringField("Not a system command, and not activated");
            target->fail();
            finishRunning(target, targetInd);
        } else {
            cmd->start();
            c->execute(cmd, target, out);
        }
    }
}

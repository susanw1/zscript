/*
 * RCodeRunner.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_RCODERUNNER_HPP_
#define SRC_TEST_CPP_RCODE_RCODERUNNER_HPP_
#include "RCodeIncludes.hpp"
#include "parsing/RCodeCommandSequence.hpp"
#include "commands/RCodeActivateCommand.hpp"

template<class RP>
class RCode;

template<class RP>
class RCodeCommandSequence;

template<class RP>
class RCodeRunner {
private:
    RCode<RP> *const rcode;
    RCodeCommandSequence<RP> *running[RP::maxParallelRunning];
    uint8_t parallelNum = 0;
    bool canBeParallel = false;

    void runSequence(RCodeCommandSequence<RP> *target, int targetInd);

    bool finishRunning(RCodeCommandSequence<RP> *target, int targetInd);

    RCodeCommandSequence<RP>* findNextToRun();
public:
    RCodeRunner(RCode<RP> *rcode) :
            rcode(rcode) {
    }

    void runNext();
};

template<class RP>
RCodeCommandSequence<RP>* RCodeRunner<RP>::findNextToRun() {
    RCodeCommandSequence <RP>*current = NULL;
    RCodeCommandChannel<RP> **channels = rcode->getChannels();
    if (canBeParallel || parallelNum == 0) {
        canBeParallel = true;
        for (int i = 0; i < rcode->getChannelNumber(); i++) {
            if (channels[i]->getCommandSequence()->isFullyParsed()
                    && channels[i]->getCommandSequence()->canBeParallel()
                    && channels[i]->getCommandSequence()->canLock()
                    && channels[i]->canLock()
                    && (!channels[i]->hasOutStream()
                            || !channels[i]->acquireOutStream()->isLocked())) {
                current = channels[i]->getCommandSequence();
                current->lock();
                channels[i]->lock();
                break;
            }
        }
    }
    if (canBeParallel && current == NULL && parallelNum == 0) {
        for (int i = 0; i < rcode->getChannelNumber(); i++) {
            if (channels[i]->getCommandSequence()->hasParsed()
                    && channels[i]->canLock()
                    && (!channels[i]->hasOutStream()
                            || !channels[i]->acquireOutStream()->isLocked())) {
                current = channels[i]->getCommandSequence();
                channels[i]->lock();
                canBeParallel = false;
                break;
            }
        }
    }
    if (current != NULL) {
        RCodeOutStream<RP> *out = current->acquireOutStream();
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
    return current;
}

template<class RP>
void RCodeRunner<RP>::runNext() {
    RCodeCommandSequence<RP> *current = NULL;
    int targetInd = 0;
    for (; targetInd < parallelNum; targetInd++) {
        if (!running[targetInd]->hasParsed()
                || running[targetInd]->peekFirst()->isComplete()
                || running[targetInd]->peekFirst()->needsMoveAlong()
                || !running[targetInd]->peekFirst()->isStarted()) {
            current = running[targetInd];
            break;
        }
    }
    if (current != NULL && current->peekFirst() != NULL && current->peekFirst()->needsMoveAlong()) {
        current->peekFirst()->setNeedsMoveAlong(false);
        current->peekFirst()->getCommand(rcode)->moveAlong(
                current->peekFirst());
        current = NULL;
    }
    if (current != NULL
            && (!current->hasParsed() || current->peekFirst()->isComplete())) {
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
    if (current == NULL && parallelNum < RP::maxParallelRunning) {
        targetInd = parallelNum;
        current = findNextToRun();
    }
    if (current != NULL && current->hasParsed()) {
        runSequence(current, targetInd);
    }
}

template<class RP>
bool RCodeRunner<RP>::finishRunning(RCodeCommandSequence<RP> *target, int targetInd) {
    if (target->hasParsed()) {
        RCodeCommandSlot<RP> *slot = target->peekFirst();
        if (slot->isStarted()) {
            slot->getCommand(rcode)->finish(slot, target->acquireOutStream());
        }
        if (slot->isStarted() && slot->getStatus() != OK) {
            if (target->fail(slot->getStatus())) {
                target->acquireOutStream()->writeCommandSequenceErrorHandler();
            } else {
                target->acquireOutStream()->writeCommandSequenceSeperator();
            }
        } else if (slot->getEnd() == '\n'
                || (target->isFullyParsed() && slot->next == NULL)) {
            target->acquireOutStream()->writeCommandSequenceSeperator();
        } else if (slot->getEnd() == '&') {
            target->acquireOutStream()->writeCommandSeperator();
        } else {
            target->fail(UNKNOWN_ERROR);
            target->acquireOutStream()->writeCommandSequenceSeperator();
        }
        if (target->hasParsed()) {
            target->popFirst();
            slot->reset();
        }
    } else if (target->isEmpty()) {
        target->acquireOutStream()->writeCommandSequenceSeperator();
        target->releaseOutStream();
    }
    if (!target->hasParsed() && target->isFullyParsed()) {
        if (!target->getChannel()->isPacketBased()
                || (target->isFullyParsed()
                        && !target->getChannel()->hasCommandSequence())) {
            target->acquireOutStream()->close();
        }
        target->releaseOutStream();
        if (!target->canBeParallel()) {
            canBeParallel = true;
        } else if (parallelNum == 1) {
            canBeParallel = true;
        }
        target->unlock();
        target->getChannel()->unlock();
        target->reset();
        for (int i = targetInd; i < parallelNum - 1; i++) {
            running[i] = running[i + 1];
        }
        parallelNum--;
        return true;
    }
    return false;
}

template<class RP>
void RCodeRunner<RP>::runSequence(RCodeCommandSequence<RP> *target, int targetInd) {
    RCodeOutStream<RP> *out = target->acquireOutStream();
    RCodeCommandSlot<RP> *cmd = target->peekFirst();
    cmd->getFields()->copyFieldTo(out, 'E');
    if (cmd->getStatus() != OK) {
        cmd->setComplete(true);
        out->writeStatus(cmd->getStatus());
        out->writeBigStringField(cmd->getErrorMessage());
    } else {
        RCodeCommand<RP> *c = cmd->getCommand(rcode);
        if (c == NULL) {
            out->writeStatus(UNKNOWN_CMD);
            out->writeBigStringField("Command not found");
            out->writeCommandSequenceSeperator();
            target->fail(UNKNOWN_CMD);
            finishRunning(target, targetInd);
        } else if (cmd->getFields()->get('R', 0xFF)
                > RCodeActivateCommand<RP>::MAX_SYSTEM_CODE
                && !RCodeActivateCommand<RP>::isActivated()) {
            out->writeStatus(NOT_ACTIVATED);
            out->writeBigStringField("Not a system command, and not activated");
            out->writeCommandSequenceSeperator();
            target->fail(NOT_ACTIVATED);
            finishRunning(target, targetInd);
        } else {
            cmd->start();
            c->execute(cmd, target, out);
        }
    }
}

#include "RCodeOutStream.hpp"
#include "RCode.hpp"

#endif /* SRC_TEST_CPP_RCODE_RCODERUNNER_HPP_ */

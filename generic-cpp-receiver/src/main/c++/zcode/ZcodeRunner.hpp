/*
 * ZcodeRunner.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODERUNNER_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODERUNNER_HPP_

#include "ZcodeIncludes.hpp"
#include "parsing/ZcodeCommandSequence.hpp"
#include "commands/ZcodeActivateCommand.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeCommandSequence;

template<class ZP>
class ZcodeRunner {
    private:
        Zcode<ZP> *const zcode;
        ZcodeCommandSequence<ZP> *running[ZP::maxParallelRunning];
        uint8_t parallelNum = 0;
        bool canBeParallel = false;

        void runSequence(ZcodeCommandSequence<ZP> *target, int targetInd);

        bool finishRunning(ZcodeCommandSequence<ZP> *target, int targetInd);

        ZcodeCommandSequence<ZP>* findNextToRun();

    public:
        ZcodeRunner(Zcode<ZP> *zcode) :
                zcode(zcode) {
        }

        void runNext();
};

template<class ZP>
ZcodeCommandSequence<ZP>* ZcodeRunner<ZP>::findNextToRun() {
    ZcodeCommandSequence<ZP> *current = NULL;
    ZcodeCommandChannel<ZP> **channels = zcode->getChannels();
    if (canBeParallel || parallelNum == 0) {
        canBeParallel = true;
        for (int i = 0; i < zcode->getChannelNumber(); i++) {
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
        for (int i = 0; i < zcode->getChannelNumber(); i++) {
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
        ZcodeOutStream<ZP> *out = current->acquireOutStream();
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

template<class ZP>
void ZcodeRunner<ZP>::runNext() {
    ZcodeCommandSequence<ZP> *current = NULL;
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
        current->peekFirst()->getCommand(zcode)->moveAlong(
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
    if (current == NULL && parallelNum < ZP::maxParallelRunning) {
        targetInd = parallelNum;
        current = findNextToRun();
    }
    if (current != NULL && current->hasParsed()) {
        runSequence(current, targetInd);
    }
}

template<class ZP>
bool ZcodeRunner<ZP>::finishRunning(ZcodeCommandSequence<ZP> *target, int targetInd) {
    if (target->hasParsed()) {
        ZcodeCommandSlot<ZP> *slot = target->peekFirst();
        if (slot->isStarted()) {
            slot->getCommand(zcode)->finish(slot, target->acquireOutStream());
        }

        if (slot->isStarted() && slot->getStatus() != OK) {
            if (target->fail(slot->getStatus())) {
                target->acquireOutStream()->writeCommandSequenceErrorHandler();
            } else {
                target->acquireOutStream()->writeCommandSequenceSeparator();
            }
        } else if (slot->getEnd() == Zchars::EOL_SYMBOL
                || (target->isFullyParsed() && slot->next == NULL)) {
            target->acquireOutStream()->writeCommandSequenceSeparator();
        } else if (slot->getEnd() == Zchars::ANDTHEN_SYMBOL) {
            target->acquireOutStream()->writeCommandSeparator();
        } else {
            target->fail(UNKNOWN_ERROR);
            target->acquireOutStream()->writeCommandSequenceSeparator();
        }

        if (target->hasParsed()) {
            target->popFirst();
            slot->reset();
        }
    } else if (target->isEmpty()) {
        target->acquireOutStream()->writeCommandSequenceSeparator();
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

template<class ZP>
void ZcodeRunner<ZP>::runSequence(ZcodeCommandSequence<ZP> *target, int targetInd) {
    ZcodeOutStream<ZP> *out = target->acquireOutStream();
    ZcodeCommandSlot<ZP> *cmd = target->peekFirst();
    cmd->getFields()->copyFieldTo(out, Zchars::ECHO_PARAM);
    if (cmd->getStatus() != OK) {
        cmd->setComplete(true);
        out->writeStatus(cmd->getStatus());
        out->writeBigStringField(cmd->getErrorMessage());
    } else {
        ZcodeCommand<ZP> *c = cmd->getCommand(zcode);
        if (c == NULL) {
            out->writeStatus(UNKNOWN_CMD);
            out->writeBigStringField("Command not found");
            out->writeCommandSequenceSeparator();
            target->fail(UNKNOWN_CMD);
            finishRunning(target, targetInd);
        } else if (cmd->getFields()->get(Zchars::CMD_PARAM, 0xFF)
                > ZcodeActivateCommand<ZP>::MAX_SYSTEM_CODE
                && !ZcodeActivateCommand<ZP>::isActivated()) {
            out->writeStatus(NOT_ACTIVATED);
            out->writeBigStringField("Not a system command, and not activated");
            out->writeCommandSequenceSeparator();
            target->fail(NOT_ACTIVATED);
            finishRunning(target, targetInd);
        } else {
            cmd->start();
            c->execute(cmd, target, out);
        }
    }
}

#include "ZcodeOutStream.hpp"
#include "Zcode.hpp"

#endif /* SRC_TEST_CPP_ZCODE_ZCODERUNNER_HPP_ */

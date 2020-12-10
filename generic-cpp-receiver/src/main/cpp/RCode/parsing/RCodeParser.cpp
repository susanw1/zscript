/*
 * RCodeParser.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeParser.hpp"

void RCodeParser::report_failure(RCodeCommandChannel *channel) {
    if (!channel->hasOutStream() || !channel->acquireOutStream()->isLocked()) {
        channel->acquireOutStream()->lock();
        channel->acquireOutStream()->openResponse(channel);
        channel->acquireOutStream()->writeStatus(SETUP_ERROR);
        channel->acquireOutStream()->writeBigStringField(
                rcode->getConfigFailureState());
        channel->acquireOutStream()->writeCommandSequenceSeperator();
        channel->acquireOutStream()->close();
        channel->acquireOutStream()->unlock();
        channel->releaseOutStream();
    }
}

void RCodeParser::parseNext() {
    RCodeCommandSlot *targetSlot = NULL;
    if (!bigBig.isInUse()) {
        for (int i = 0; i < RCodeParameters::slotNum; i++) {
            if (!slots[i].isParsed()) {
                targetSlot = &slots[i];
                break;
            }
        }
        if (targetSlot != NULL) {
            if (mostRecent != NULL && mostRecent->canContinueParse()) {
                parse(targetSlot, mostRecent);
            } else {
                if (mostRecent != NULL) {
                    mostRecent->unsetFailed();
                    mostRecent->releaseInStream();
                    mostRecent->getChannel()->unlock();
                }
                mostRecent = NULL;
                for (int i = 0; i < rcode->getChannelNumber(); i++) {
                    RCodeCommandChannel *channel = rcode->getChannels()[i];
                    if (channel->canLock() && channel->hasCommandSequence()
                            && !channel->getCommandSequence()->isActive()
                            && (!channel->hasInStream()
                                    || !channel->acquireInStream()->getSequenceInStream()->isLocked())) {
                        if (rcode->getConfigFailureState() == NULL) {
                            mostRecent = beginSequenceParse(targetSlot,
                                    channel);
                        } else {
                            channel->acquireInStream()->getSequenceInStream()->open();
                            channel->acquireInStream()->getSequenceInStream()->close();
                            report_failure(channel);
                            channel->releaseInStream();
                        }
                        break;
                    }
                }
            }
        }
    }
}

RCodeCommandSequence* RCodeParser::beginSequenceParse(
        RCodeCommandSlot *targetSlot, RCodeCommandChannel *channel) {

    RCodeCommandSequence *candidateSequence = channel->getCommandSequence();
    channel->lock();
    candidateSequence->acquireInStream()->open();
    if (candidateSequence->parseFlags()) { // returns false if a debug sequence, true otherwise
        candidateSequence->setActive();
        if (!candidateSequence->isFullyParsed()) {
            // This is the normal case - nothing unexpected has happened
            parse(targetSlot, candidateSequence);
            return candidateSequence;
        } else {
            // This is when the incoming command sequence (candidateSequence) is empty / has only markers
            channel->unlock();
            candidateSequence->releaseInStream();
            return NULL;
        }
    } else {
        // The received sequence is debug - we will ignore it
        candidateSequence->releaseInStream();
        channel->unlock();
        candidateSequence->reset();
        return NULL;
    }
}
void RCodeParser::parse(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence) {
    bool worked = slot->parseSingleCommand(
            sequence->acquireInStream()->getCommandInStream(), sequence);
    sequence->addLast(slot);
    if (!worked) {
        sequence->acquireInStream()->close();
    }
    if (!worked || slot->isEndOfSequence()) {
        sequence->setFullyParsed(true);
        sequence->releaseInStream();
        sequence->getChannel()->unlock();
    }
}

void RCodeParserSetupSlots(RCodeParser *parser) {
    for (int i = 0; i < RCodeParameters::slotNum; ++i) {
        parser->slots[i].setup(&parser->bigBig);
    }
}

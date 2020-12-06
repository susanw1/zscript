/*
 * RCodeParser.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeParser.hpp"

void RCodeParser::eatWhitespace(RCodeInStream *in) {
    char c = in->peek();
    while (in->hasNext() && (c == ' ' || c == '\t' || c == '\r')) {
        in->read();
        c = in->peek();
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
                            && !channel->getInStream()->isLocked()) {
                        mostRecent = beginSequenceParse(targetSlot, channel);
                        break;
                    }
                }
            }
        }
    }
}

RCodeCommandSequence* RCodeParser::beginSequenceParse(
        RCodeCommandSlot *targetSlot, RCodeCommandChannel *channel) {

    RCodeCommandSequence *seq = channel->getCommandSequence();
    channel->lock();
    seq->acquireInStream()->getSequenceIn()->openCommandSequence();
    if (seq->parseFlags()) { // returns false if a debug sequence, true otherwise
        seq->setActive();
        if (!seq->isFullyParsed()) {
            // This is the normal case - nothing unexpected has happened
            parse(targetSlot, seq);
            return seq;
        } else {
            // This is when the incoming command sequence (candidateSequence) is empty / has only markers
            channel->unlock();
            seq->releaseInStream();
            return NULL;
        }
    } else {
        // The received sequence is debug - we will ignore it
        seq->releaseInStream();
        channel->unlock();
        seq->reset();
        return NULL;
    }
}
void RCodeParser::parse(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence) {
    bool worked = slot->parseSingleCommand(sequence->acquireInStream(),
            sequence);
    sequence->addLast(slot);
    if (!worked) {
        sequence->acquireInStream()->skipSequence();
    }
    if (!worked || slot->getEnd() == '\n') {
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

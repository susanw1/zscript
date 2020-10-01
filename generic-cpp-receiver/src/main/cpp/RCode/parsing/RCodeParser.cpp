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

bool RCodeParser::shouldEatWhitespace(RCodeInStream *in) {
    RCodeLookaheadStream *l = in->getLookahead();
    char c = l->read();
    while (c == ' ' || c == '\t' || c == '\r') {
        c = l->read();
    }
    return c != '&' && c != '|';
}

void RCodeParser::parseNext() {
    RCodeCommandSlot *targetSlot = NULL;
    if (bigBig.getLength() == 0) {
        for (int i = 0; i < RCodeParameters::slotNum; i++) {
            if (!slots[i].isParsed()) {
                targetSlot = &slots[i];
                break;
            }
        }
        if (targetSlot != NULL) {
            if (mostRecent != NULL && mostRecent->isActive()
                    && !mostRecent->isFullyParsed()
                    && !mostRecent->hasFailed()) {
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
                            && !channel->getCommandSequence()->isActive()) {
                        RCodeCommandSequence *seq =
                                channel->getCommandSequence();
                        if (!channel->getInStream()->isLocked()) {
                            channel->lock();
                            seq->getInStream()->getSequenceIn()->openCommandSequence();
                            if (seq->parseFlags()) {
                                if (seq->isFullyParsed()) {
                                    channel->unlock();
                                    seq->setActive();
                                    seq->releaseInStream();
                                } else {
                                    mostRecent = seq;
                                    parse(targetSlot, seq);
                                    seq->setActive();
                                }
                            } else {
                                seq->releaseInStream();
                                channel->unlock();
                                seq->reset();
                            }
                            break;
                        }
                    }
                }
            }
        }
    }
}
void RCodeParser::parse(RCodeCommandSlot *slot,
        RCodeCommandSequence *sequence) {
    if (!sequence->getInStream()->getSequenceIn()->hasNext()) {
        sequence->setFullyParsed(true);
        sequence->releaseInStream();
        sequence->getChannel()->unlock();
    } else {
        bool worked = slot->parseSingleCommand(sequence->getInStream(),
                sequence);
        if (!worked) {
            sequence->addLast(slot);
            sequence->getInStream()->skipSequence();
            sequence->setFailed();
            sequence->setFullyParsed(true);
            sequence->releaseInStream();
            sequence->getChannel()->unlock();
        } else {
            sequence->addLast(slot);
            if (slot->getEnd() == '\n') {
                sequence->setFullyParsed(true);
                sequence->releaseInStream();
                sequence->getChannel()->unlock();
            }
        }
    }
}

void RCodeParserSetupSlots(RCodeParser *parser) {
    for (int i = 0; i < RCodeParameters::slotNum; ++i) {
        parser->slots[i].setup(&parser->bigBig);
    }
}

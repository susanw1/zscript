/*
 * RCodeParser.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeParser.hpp"

#include <iostream>

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
                }
                mostRecent = NULL;
                for (int i = 0; i < rcode->getChannelNumber(); i++) {
                    RCodeCommandChannel *channel = rcode->getChannels()[i];
                    if (channel->hasCommandSequence()
                            && !channel->getCommandSequence()->isActive()) {
                        RCodeCommandSequence *seq =
                                channel->getCommandSequence();
                        if (!channel->getInStream()->isLocked()) {
                            seq->getInStream()->getSequenceIn()->openCommandSequence();
                            mostRecent = seq;
                            parse(targetSlot, seq);
                            seq->setActive();
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
    } else {
        bool worked = slot->parseSingleCommand(sequence->getInStream(),
                sequence);
        if (!worked) {
            if (slot->getStatus() == SKIP_COMMAND) {
                sequence->addLast(slot);
                sequence->getInStream()->skipSequence();
                sequence->setFullyParsed(true);
            } else {
                sequence->addLast(slot);
                sequence->getInStream()->skipSequence();
                sequence->setFailed();
            }
            sequence->releaseInStream();
        } else {
            sequence->addLast(slot);
            if (slot->getEnd() == '\n'
                    || !slot->getCommand(rcode)->continueLocking(
                            sequence->getChannel())) {
                sequence->setFullyParsed(true);
                sequence->releaseInStream();
            }
        }
    }
}

void RCodeParserSetupSlots(RCodeParser *parser) {
    for (int i = 0; i < RCodeParameters::slotNum; ++i) {
        parser->slots[i].setup(&parser->bigBig);
    }
}

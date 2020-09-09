/*
 * RCodeParser.cpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#include "RCodeParser.hpp"

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
            if (mostRecent != NULL && !mostRecent->isFullyParsed()) {
                parse(targetSlot, mostRecent);
            } else {
                mostRecent = NULL;
                for (int i = 0; i < rcode->getChannelNumber(); i++) {
                    RCodeCommandChannel *ch = &rcode->getChannels()[i];
                    if (ch->hasCommandSequence() && ch->getCommandSequence()->peekFirst()
                    == NULL) {
                        RCodeCommandSequence *seq = ch->getCommandSequence();
                        if (!ch->getInStream()->isLocked()) {
                            seq->getInStream()->getSequenceIn()->openCommandSequence();
                            mostRecent = seq;
                            parse(targetSlot, seq);
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
            if (slot->getStatus() != SKIP_COMMAND) {
                sequence->addLast(slot);
            }
            sequence->getInStream()->skipSequence();
            sequence->setFullyParsed(true);
            sequence->releaseInStream();
        } else {
            sequence->addLast(slot);
            if (slot->getEnd() == '\n'
                    || !slot->getCommand()->continueLocking(
                            sequence->getChannel())) {
                sequence->setFullyParsed(true);
                sequence->releaseInStream();
            }
        }
    }
}

void RCodeParserSetupSlots(RCodeParser *parser) {
    for (int i = 0; i < RCodeParameters::slotNum; ++i) {
        parser->slots[i].setup(parser->rcode, &parser->bigBig);
    }
}

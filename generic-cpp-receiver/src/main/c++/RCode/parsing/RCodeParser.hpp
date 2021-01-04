/*
 * RCodeParser.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_RCODE_PARSING_RCODEPARSER_HPP_
#define SRC_TEST_CPP_RCODE_PARSING_RCODEPARSER_HPP_
#include "../RCodeIncludes.hpp"
#include "RCodeBigField.hpp"
#include "RCodeCommandSlot.hpp"

template<class RP>
class RCode;

template<class RP>
class RCodeCommandSequence;

template<class RP>
class RCodeCommandChannel;

template<class RP>
class RCodeParser {
private:
    RCode<RP> *const rcode;
    RCodeCommandSequence<RP> *mostRecent = NULL;
    RCodeCommandSlot<RP> slots[RP::slotNum];
    RCodeBigBigField<RP> bigBig;

    RCodeCommandSequence<RP>* beginSequenceParse(RCodeCommandSlot<RP> *targetSlot, RCodeCommandChannel<RP> *channel);
    void parse(RCodeCommandSlot<RP> *slot, RCodeCommandSequence<RP> *sequence);

    void report_failure(RCodeCommandChannel<RP> *channel);
public:
    RCodeParser(RCode<RP> *rcode) :
            rcode(rcode) {
        setupSlots();
    }

    void parseNext();

    void setupSlots();
};

template<class RP>
void RCodeParser<RP>::setupSlots() {
    for (int i = 0; i < RP::slotNum; ++i) {
        slots[i].setup(&bigBig);
    }
}

template<class RP>
void RCodeParser<RP>::report_failure(RCodeCommandChannel<RP> *channel) {
    if (!channel->hasOutStream() || !channel->acquireOutStream()->isLocked()) {
        channel->acquireOutStream()->lock();
        channel->acquireOutStream()->openResponse(channel);
        channel->acquireOutStream()->writeStatus(SETUP_ERROR);
        channel->acquireOutStream()->writeBigStringField(rcode->getConfigFailureState());
        channel->acquireOutStream()->writeCommandSequenceSeperator();
        channel->acquireOutStream()->close();
        channel->acquireOutStream()->unlock();
        channel->releaseOutStream();
    }
}

template<class RP>
void RCodeParser<RP>::parseNext() {
    RCodeCommandSlot<RP> *targetSlot = NULL;
    if (!bigBig.isInUse()) {
        for (int i = 0; i < RP::slotNum; i++) {
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
                    RCodeCommandChannel<RP> *channel = rcode->getChannels()[i];
                    if (channel->canLock() && channel->hasCommandSequence() && !channel->getCommandSequence()->isActive()
                            && (!channel->hasInStream() || !channel->acquireInStream()->getSequenceInStream()->isLocked())) {
                        if (rcode->getConfigFailureState() == NULL) {
                            mostRecent = beginSequenceParse(targetSlot, channel);
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

template<class RP>
RCodeCommandSequence<RP>* RCodeParser<RP>::beginSequenceParse(RCodeCommandSlot<RP> *targetSlot, RCodeCommandChannel<RP> *channel) {
    RCodeCommandSequence<RP> *candidateSequence = channel->getCommandSequence();
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

template<class RP>
void RCodeParser<RP>::parse(RCodeCommandSlot<RP> *slot, RCodeCommandSequence <RP>*sequence) {
    bool worked = slot->parseSingleCommand(sequence->acquireInStream()->getCommandInStream(), sequence);
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



#include "../RCode.hpp"
#include "RCodeCommandChannel.hpp"
#include "RCodeCommandSequence.hpp"

#endif /* SRC_TEST_CPP_RCODE_PARSING_RCODEPARSER_HPP_ */

/*
 * ZcodeParser.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_
#include "../ZcodeIncludes.hpp"
#include "ZcodeBigField.hpp"
#include "ZcodeCommandSlot.hpp"

template<class RP>
class Zcode;

template<class RP>
class ZcodeCommandSequence;

template<class RP>
class ZcodeCommandChannel;

template<class RP>
class ZcodeParser {
private:
    Zcode<RP> *const zcode;
    ZcodeCommandSequence<RP> *mostRecent = NULL;
    ZcodeCommandSlot<RP> slots[RP::slotNum];
    ZcodeBigBigField<RP> bigBig;

    ZcodeCommandSequence<RP>* beginSequenceParse(ZcodeCommandSlot<RP> *targetSlot, ZcodeCommandChannel<RP> *channel);
    void parse(ZcodeCommandSlot<RP> *slot, ZcodeCommandSequence<RP> *sequence);

    void report_failure(ZcodeCommandChannel<RP> *channel);
public:
    ZcodeParser(Zcode<RP> *zcode) :
            zcode(zcode) {
        setupSlots();
    }

    void parseNext();

    void setupSlots();
};

template<class RP>
void ZcodeParser<RP>::setupSlots() {
    for (int i = 0; i < RP::slotNum; ++i) {
        slots[i].setup(&bigBig);
    }
}

template<class RP>
void ZcodeParser<RP>::report_failure(ZcodeCommandChannel<RP> *channel) {
    if (!channel->hasOutStream() || !channel->acquireOutStream()->isLocked()) {
        channel->acquireOutStream()->lock();
        channel->acquireOutStream()->openResponse(channel);
        channel->acquireOutStream()->writeStatus(SETUP_ERROR);
        channel->acquireOutStream()->writeBigStringField(zcode->getConfigFailureState());
        channel->acquireOutStream()->writeCommandSequenceSeperator();
        channel->acquireOutStream()->close();
        channel->acquireOutStream()->unlock();
        channel->releaseOutStream();
    }
}

template<class RP>
void ZcodeParser<RP>::parseNext() {
    ZcodeCommandSlot<RP> *targetSlot = NULL;
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
                for (int i = 0; i < zcode->getChannelNumber(); i++) {
                    ZcodeCommandChannel<RP> *channel = zcode->getChannels()[i];
                    if (channel->canLock() && channel->hasCommandSequence() && !channel->getCommandSequence()->isActive()
                            && (!channel->hasInStream() || !channel->acquireInStream()->getSequenceInStream()->isLocked())) {
                        if (zcode->getConfigFailureState() == NULL) {
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
ZcodeCommandSequence<RP>* ZcodeParser<RP>::beginSequenceParse(ZcodeCommandSlot<RP> *targetSlot, ZcodeCommandChannel<RP> *channel) {
    ZcodeCommandSequence<RP> *candidateSequence = channel->getCommandSequence();
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
void ZcodeParser<RP>::parse(ZcodeCommandSlot<RP> *slot, ZcodeCommandSequence <RP>*sequence) {
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



#include "../Zcode.hpp"
#include "ZcodeCommandChannel.hpp"
#include "ZcodeCommandSequence.hpp"

#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODEPARSER_HPP_ */

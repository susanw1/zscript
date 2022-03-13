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

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeCommandSequence;

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeParser {
private:
    Zcode<ZP> *const zcode;
    ZcodeCommandSequence<ZP> *mostRecent = NULL;
    ZcodeCommandSlot<ZP> slots[ZP::slotNum];
    ZcodeHugeField<ZP> huge;

    ZcodeCommandSequence<ZP>* beginSequenceParse(ZcodeCommandSlot<ZP> *targetSlot, ZcodeCommandChannel<ZP> *channel);
    void parse(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence<ZP> *sequence);

    void report_failure(ZcodeCommandChannel<ZP> *channel);
public:
    ZcodeParser(Zcode<ZP> *zcode) :
            zcode(zcode) {
        setupSlots();
    }

    void parseNext();

    void setupSlots();
};

template<class ZP>
void ZcodeParser<ZP>::setupSlots() {
    for (int i = 0; i < ZP::slotNum; ++i) {
        slots[i].setup(&huge);
    }
}

template<class ZP>
void ZcodeParser<ZP>::report_failure(ZcodeCommandChannel<ZP> *channel) {
    if (!channel->hasOutStream() || !channel->acquireOutStream()->isLocked()) {
        channel->acquireOutStream()->lock();
        channel->acquireOutStream()->openResponse(channel);
        channel->acquireOutStream()->writeStatus(SETUP_ERROR);
        channel->acquireOutStream()->writeBigStringField(zcode->getConfigFailureState());
        channel->acquireOutStream()->writeCommandSequenceSeparator();
        channel->acquireOutStream()->close();
        channel->acquireOutStream()->unlock();
        channel->releaseOutStream();
    }
}

template<class ZP>
void ZcodeParser<ZP>::parseNext() {
    ZcodeCommandSlot<ZP> *targetSlot = NULL;
    if (!huge.isInUse()) {
        for (int i = 0; i < ZP::slotNum; i++) {
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
                    ZcodeCommandChannel<ZP> *channel = zcode->getChannels()[i];
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

template<class ZP>
ZcodeCommandSequence<ZP>* ZcodeParser<ZP>::beginSequenceParse(ZcodeCommandSlot<ZP> *targetSlot, ZcodeCommandChannel<ZP> *channel) {
    ZcodeCommandSequence<ZP> *candidateSequence = channel->getCommandSequence();
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

template<class ZP>
void ZcodeParser<ZP>::parse(ZcodeCommandSlot<ZP> *slot, ZcodeCommandSequence <ZP>*sequence) {
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

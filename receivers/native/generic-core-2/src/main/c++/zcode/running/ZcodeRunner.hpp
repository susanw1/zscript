/*
 * ZcodeRunner.hpp
 *
 *  Created on: 9 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_ZCODERUNNER_HPP_
#define SRC_TEST_CPP_ZCODE_ZCODERUNNER_HPP_

#include "../ZcodeIncludes.hpp"
#include "ZcodeRunningCommandSlotFinderInc.hpp"

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeRunner {
private:
    Zcode<ZP> *zcode;

public:
    ZcodeRunner(Zcode<ZP> *zcode) :
            zcode(zcode) {
    }

    void runNext() {
        ZcodeRunningCommandSlot<ZP> *target = NULL;
        for (uint8_t i = 0; i < zcode->getChannelNumber(); ++i) {
            ZcodeRunningCommandSlot<ZP> *runner = &zcode->getChannels()[i]->runner;
            if (runner->needsRunAction() && runner->lock()) {
                target = runner;
                break;
            } else {
                runner->checkSeqEnd();
            }
        }
        if (target != NULL) {
            if (target->isAtSequenceStart()) {
                target->startSequence();
            }
            target->performRunAction();
        }
    }
};

#endif /* SRC_TEST_CPP_ZCODE_ZCODERUNNER_HPP_ */

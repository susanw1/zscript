/*
 * ZcodeRunningCommandSlotFinderInc.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODERUNNINGCOMMANDSLOT_FINDERINC_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODERUNNINGCOMMANDSLOT_FINDERINC_HPP_
#include "ZcodeRunningCommandSlot.hpp"
#include "ZcodeCommandFinder.hpp"

template<class ZP>
void ZcodeRunningCommandSlot<ZP>::performRunAction() {
    if (!status.isStarted) {
        if (!status.hasWrittenTerminator) {
            writeTerminator(commandSlot->starter);
        }
        dataPointer = NULL;
        storedData = 0;
        status.isStarted = true;
    }
    status.needsAction = false;
    ZcodeCommandFinder<ZP>::runCommand(this);
    if (status.isComplete) {
        if (commandSlot->terminator == EOL_SYMBOL) {
            writeTerminator(EOL_SYMBOL);
        } else {
            status.hasWrittenTerminator = false;
        }
        finish();
    }
}
#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODERUNNINGCOMMANDSLOT_FINDERINC_HPP_ */

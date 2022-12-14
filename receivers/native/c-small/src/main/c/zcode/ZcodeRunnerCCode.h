/*
 * ZcodeRunner.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODERUNNERCCODE_H_
#define SRC_MAIN_C_ZCODE_ZCODERUNNERCCODE_H_
#include "Zcode.h"
#include "ZcodeOutStream.h"
#include "parsing/ZcodeCommandSlotCCode.h"

void ZcodeRunnerPerformFail();
#include "ZcodeCommandFinderCCode.h"

void ZCodeRunnerWriteTerminator(char term) {
    ZcodeCommandSlot *slot = &zcode.slot;
    slot->runStatus.hasWrittenTerminator = true;
    if (term == EOL_SYMBOL) {
        ZcodeOutStream_WriteCommandSequenceSeparator();
    } else if (term == ANDTHEN_SYMBOL) {
        ZcodeOutStream_WriteCommandSeparator();
    } else if (term == ORELSE_SYMBOL) {
        ZcodeOutStream_WriteSequenceErrorHandler();
    }
}

void ZcodeRunNext() {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (slot->state.waitingOnRun && (slot->runStatus.hasOutLock || ZcodeOutStream_Lock())) {
        slot->runStatus.hasOutLock = true;
        if (slot->runStatus.isFirstCommand) {
            ZcodeOutStream_Open();
            if (slot->runStatus.isBroadcast) {
                ZcodeOutStream_MarkBroadcast();
            }
        }
        if (!slot->runStatus.hasWrittenTerminator) {
            ZCodeRunnerWriteTerminator(slot->starter);
        }
        slot->runStatus.hasWrittenTerminator = false;
        runZcodeCommand();
        if (!slot->runStatus.hasWrittenTerminator) {
            if (slot->terminator == EOL_SYMBOL) {
                ZCodeRunnerWriteTerminator(EOL_SYMBOL);
            } else {
                slot->runStatus.hasWrittenTerminator = false;
            }
        }
        ZcodeCommandSlot_finish();
    } else {
        if (slot->runStatus.hasOutLock && slot->runStatus.isFirstCommand) {
            if (!slot->runStatus.hasWrittenTerminator) {
                ZCodeRunnerWriteTerminator(EOL_SYMBOL);
            }
            ZcodeOutStream_Unlock();
            slot->runStatus.hasOutLock = false;
            ZcodeOutStream_Close();
        }
    }
}

void ZcodeCommandQuietEnd() {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (slot->runStatus.isFirstCommand && slot->terminator == EOL_SYMBOL) {
        slot->runStatus.quietEnd = !slot->runStatus.hasWrittenTerminator;
        slot->runStatus.hasWrittenTerminator = true;
    }
}

void ZcodeCommandMildFail(ZcodeResponseStatus failStatus) {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (slot->runStatus.quietEnd) {
        slot->runStatus.hasWrittenTerminator = false;
    }
    ZcodeCommandSlot_failExternal(failStatus);
}

void ZcodeCommandFail(ZcodeResponseStatus failStatus) {
    ZcodeCommandMildFail(failStatus);
    ZcodeOutStream_WriteStatus(failStatus);
}

void ZcodeCommandFailWithMessage(ZcodeResponseStatus failStatus, const char *message) {
    ZcodeCommandMildFail(failStatus);
    ZcodeOutStream_WriteStatus(failStatus);
    if (message != NULL && message[0] != 0) {
        ZcodeOutStream_WriteBigStringField(message);
    }
}

void ZcodeRunnerPerformFail() {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (slot->runStatus.quietEnd) {
        slot->runStatus.hasWrittenTerminator = false;
    }
    ZcodeOutStream_WriteStatus((ZcodeResponseStatus) slot->respStatus);
    ZCodeRunnerWriteTerminator(EOL_SYMBOL);
    ZcodeCommandSlot_finish();
    slot->runStatus.quietEnd = false;
}
#endif /* SRC_MAIN_C_ZCODE_ZCODERUNNERCCODE_H_ */

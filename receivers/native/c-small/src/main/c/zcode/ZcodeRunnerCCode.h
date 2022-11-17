/*
 * ZcodeRunner.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODERUNNERCCODE_H_
#define SRC_MAIN_C_ZCODE_ZCODERUNNERCCODE_H_
#include "Zcode.h"
#include "parsing/ZcodeCommandSlotCCode.h"
#include "ZcodeCommandFinderCCode.h"

void ZCodeRunnerWriteTerminator(ZcodeCommandSlot *commandSlot, char term) {
    commandSlot->runStatus.hasWrittenTerminator = true;
    if (term == EOL_SYMBOL) {
        ZcodeOutStream_WriteSequenceSeperator();
    } else if (term == ANDTHEN_SYMBOL) {
        ZcodeOutStream_WriteCommandSeperator();
    } else if (term == ORELSE_SYMBOL) {
        ZcodeOutStream_WriteSequenceErrorHandler();
    }
}

void ZcodeRunNext(Zcode *zcode) {
    ZcodeCommandSlot *commandSlot = &zcode->slot;
    if (commandSlot->state.waitingOnRun && (commandSlot->runStatus.hasOutLock || ZcodeOutStream_Lock())) {
        if (commandSlot->runStatus.isFirstCommand) {
            ZcodeOutStream_OpenResponse();
            if (commandSlot->runStatus.isBroadcast) {
                ZcodeOutStream_MarkBroadcast();
            }
        }
        if (!commandSlot->runStatus.hasWrittenTerminator) {
            ZCodeRunnerWriteTerminator(commandSlot, commandSlot->starter);
        }
        commandSlot->runStatus.hasWrittenTerminator = false;
        runZcodeCommand(zcode);
        if (!commandSlot->runStatus.hasWrittenTerminator) {
            if (commandSlot->terminator == EOL_SYMBOL) {
                ZCodeRunnerWriteTerminator(commandSlot, EOL_SYMBOL);
            } else {
                commandSlot->runStatus.hasWrittenTerminator = false;
            }
        }
        ZcodeCommandSlot__finish(commandSlot);
    } else {
        if (commandSlot->runStatus.hasOutLock && commandSlot->runStatus.isFirstCommand) {
            if (!commandSlot->runStatus.hasWrittenTerminator) {
                ZCodeRunnerWriteTerminator(commandSlot, EOL_SYMBOL);
            }
            if (commandSlot->runStatus.hasOutLock) {
                ZcodeOutStream_Unlock();
            }
            ZcodeOutStream_Close();
        }
    }
}

void ZcodeCommandFail(ZcodeCommandSlot *commandSlot, ZcodeResponseStatus failStatus) {
    if (commandSlot->runStatus.quietEnd) {
        commandSlot->runStatus.hasWrittenTerminator = false;
    }
    if (!commandSlot->runStatus.hasWrittenTerminator) {
        ZCodeRunnerWriteTerminator(commandSlot->starter);
    }
    ZcodeCommandSlot__failExternal(commandSlot, failStatus);
    ZcodeOutStream_WriteStatus(failStatus);
    if (commandSlot->terminator == EOL_SYMBOL) {
        ZCodeRunnerWriteTerminator(EOL_SYMBOL);
    } else {
        commandSlot->runStatus.hasWrittenTerminator = false;
    }
    ZcodeCommandSlot__finish(commandSlot);
    commandSlot->runStatus.quietEnd = false;
}

void ZcodeCommandQuietEnd(ZcodeCommandSlot *commandSlot) {
    if (commandSlot->runStatus.isFirstCommand && commandSlot->terminator == EOL_SYMBOL) {
        commandSlot->runStatus.quietEnd = !commandSlot->runStatus.hasWrittenTerminator;
        commandSlot->runStatus.hasWrittenTerminator = true;
    }
}

void ZcodeCommandMildFail(ZcodeCommandSlot *commandSlot, ZcodeResponseStatus failStatus) {
    if (commandSlot->runStatus.quietEnd) {
        commandSlot->runStatus.hasWrittenTerminator = false;
    }
    ZcodeCommandSlot__failExternal(commandSlot, failStatus);
}

void ZcodeRunnerPerformFail(ZcodeCommandSlot *commandSlot) {
    if (commandSlot->runStatus.quietEnd) {
        commandSlot->runStatus.hasWrittenTerminator = false;
    }
    ZcodeOutStream_WriteStatus((ZcodeResponseStatus) commandSlot->respStatus);
    ZCodeRunnerWriteTerminator(EOL_SYMBOL);
    ZcodeCommandSlot__finish(commandSlot);
    commandSlot->runStatus.quietEnd = false;
}
#endif /* SRC_MAIN_C_ZCODE_ZCODERUNNERCCODE_H_ */

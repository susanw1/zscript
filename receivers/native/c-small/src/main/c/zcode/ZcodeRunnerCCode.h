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

void ZcodeRunnerPerformFail(ZcodeCommandSlot *commandSlot);
#include "ZcodeCommandFinderCCode.h"

void ZCodeRunnerWriteTerminator(ZcodeCommandSlot *commandSlot, char term) {
    commandSlot->runStatus.hasWrittenTerminator = true;
    if (term == EOL_SYMBOL) {
        ZcodeOutStream_WriteCommandSequenceSeparator();
    } else if (term == ANDTHEN_SYMBOL) {
        ZcodeOutStream_WriteCommandSeparator();
    } else if (term == ORELSE_SYMBOL) {
        ZcodeOutStream_WriteSequenceErrorHandler();
    }
}

void ZcodeRunNext(Zcode *zcode) {
    ZcodeCommandSlot *commandSlot = &zcode->slot;
    if (commandSlot->state.waitingOnRun && (commandSlot->runStatus.hasOutLock || ZcodeOutStream_Lock())) {
        commandSlot->runStatus.hasOutLock = true;
        if (commandSlot->runStatus.isFirstCommand) {
            ZcodeOutStream_Open();
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
        ZcodeCommandSlot_finish(commandSlot);
    } else {
        if (commandSlot->runStatus.hasOutLock && commandSlot->runStatus.isFirstCommand) {
            if (!commandSlot->runStatus.hasWrittenTerminator) {
                ZCodeRunnerWriteTerminator(commandSlot, EOL_SYMBOL);
            }
            ZcodeOutStream_Unlock();
            commandSlot->runStatus.hasOutLock = false;
            ZcodeOutStream_Close();
        }
    }
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
    ZcodeCommandSlot_failExternal(commandSlot, failStatus);
}

void ZcodeCommandFail(ZcodeCommandSlot *commandSlot, ZcodeResponseStatus failStatus) {
    ZcodeCommandMildFail(commandSlot, failStatus);
    ZcodeOutStream_WriteStatus(failStatus);
}

void ZcodeCommandFailWithMessage(ZcodeCommandSlot *commandSlot, ZcodeResponseStatus failStatus, const char *message) {
    ZcodeCommandMildFail(commandSlot, failStatus);
    ZcodeOutStream_WriteStatus(failStatus);
    if (message != NULL && message[0] != 0) {
        ZcodeOutStream_WriteBigStringField(message);
    }
}

void ZcodeRunnerPerformFail(ZcodeCommandSlot *commandSlot) {
    if (commandSlot->runStatus.quietEnd) {
        commandSlot->runStatus.hasWrittenTerminator = false;
    }
    ZcodeOutStream_WriteStatus((ZcodeResponseStatus) commandSlot->respStatus);
    ZCodeRunnerWriteTerminator(commandSlot, EOL_SYMBOL);
    ZcodeCommandSlot_finish(commandSlot);
    commandSlot->runStatus.quietEnd = false;
}
#endif /* SRC_MAIN_C_ZCODE_ZCODERUNNERCCODE_H_ */

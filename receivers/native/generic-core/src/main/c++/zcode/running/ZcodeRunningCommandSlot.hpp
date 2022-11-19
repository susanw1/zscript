/*
 * Zcode Library - Command System for Microcontrollers)
 * Copyright (c) 2022 Zcode team (Susan Witts, Alicia Witts)
 *
 * SPDX-License-Identifier: MIT
 */

#ifndef SRC_MAIN_CPP_ZCODE_RUNNING_ZCODERUNNINGCOMMANDSLOT_HPP_
#define SRC_MAIN_CPP_ZCODE_RUNNING_ZCODERUNNINGCOMMANDSLOT_HPP_

#include "../ZcodeIncludes.hpp"
#include "../parsing/ZcodeChannelCommandSlot.hpp"

struct ZcodeRunningCommandSlotStatus {
    bool isComplete :1;
    bool isStarted :1;
    bool hasWrittenTerminator :1;
    bool hasOutLock :1;
    bool needsAction :1;
    bool quietEnd :1;

    void resetCommand() {
        isComplete = false;
        isStarted = false;
        needsAction = false;
        quietEnd = false;
    }
    void fullReset() {
        isComplete = false;
        isStarted = false;
        hasWrittenTerminator = true;
        hasOutLock = false;
        needsAction = false;
        quietEnd = false;
    }
};

template<class ZP>
class ZcodeExecutionCommandSlot;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeRunningCommandSlot {
    typedef typename ZP::Strings::string_t string_t;

    friend class ZcodeExecutionCommandSlot<ZP> ;
    ZcodeChannelCommandSlot<ZP> *commandSlot;
    ZcodeOutStream<ZP> *out;

public:
    volatile uint8_t *dataPointer;
    volatile uint32_t storedData;

private:
    ZcodeRunningCommandSlotStatus status;

    void writeTerminator(char term) {
        status.hasWrittenTerminator = true;
        if (term == EOL_SYMBOL) {
            out->writeCommandSequenceSeparator();
        } else if (term == ANDTHEN_SYMBOL) {
            out->writeCommandSeparator();
        } else if (term == ORELSE_SYMBOL) {
            out->writeCommandSequenceErrorHandler();
        }
    }

public:

    ZcodeRunningCommandSlot(ZcodeOutStream<ZP> *out, ZcodeChannelCommandSlot<ZP> *commandSlot) :
            commandSlot(commandSlot), out(out), dataPointer(NULL), storedData(0), status() {
        status.fullReset();
    }

    Zcode<ZP>* getZcode() {
        return &Zcode<ZP>::zcode;
    }

    bool failedParse() {
        return commandSlot->state.hasFailed;
    }
    bool isEmpty() {
        return commandSlot->fieldMap.getFieldCount() == 0 && commandSlot->bigField.getLength() == 0;
    }
    const ZcodeFieldMap<ZP>* getFields() {
        return &commandSlot->fieldMap;
    }

    const ZcodeBigField<ZP>* getBig() {
        return &commandSlot->bigField;
    }

    bool needsRunAction() {
        return (commandSlot->state.waitingOnRun && !status.isStarted) || status.needsAction;
    }

    void performRunAction();

    bool lock() {
        status.hasOutLock = status.hasOutLock || out->lock();
        if (!status.hasOutLock) {
            return false;
        }
        if (!commandSlot->lockSet.lock()) {
            out->unlock();
            status.hasOutLock = false;
            return false;
        }
        return true;
    }

    void unlock() {
        commandSlot->lockSet.unlock();
        if (status.hasOutLock) {
            out->unlock();
            status.hasOutLock = false;
        }
    }

    bool isAtSequenceStart() {
        return commandSlot->runStatus.isFirstCommand && !status.isStarted;
    }
    void startSequence() {
        out->openResponse(commandSlot->channel);
        if (commandSlot->runStatus.isBroadcast) {
            out->markBroadcast();
        }
    }

    void checkSeqEnd() {
        if (status.hasOutLock && !status.isStarted && commandSlot->runStatus.isFirstCommand) {
            if (!status.hasWrittenTerminator) {
                writeTerminator(EOL_SYMBOL);
            }
            unlock();
            if (!commandSlot->channel->packetBased || !commandSlot->runStatus.hasData) {
                //tries to put multiple responses into the same packet
                out->close();
            }
        }
    }
    void quietEnd() {
        if (commandSlot->runStatus.isFirstCommand && commandSlot->terminator == EOL_SYMBOL) {
            status.quietEnd = !status.hasWrittenTerminator;
            status.hasWrittenTerminator = true;
        }
    }

    void fail(ZcodeResponseStatus failStatus, string_t message) {
        if (status.quietEnd) {
            status.hasWrittenTerminator = false;
        }
        status.isComplete = true;
        if (!status.hasWrittenTerminator) {
            writeTerminator(commandSlot->starter);
        }
        commandSlot->failExternal(failStatus);
        out->writeStatus(failStatus);
        if (message != NULL && ZP::Strings::getChar(message, 0)) {
            out->writeBigStringField(message);
        }
        if (commandSlot->terminator == EOL_SYMBOL) {
            writeTerminator(EOL_SYMBOL);
        } else {
            status.hasWrittenTerminator = false;
        }
        finish();
    }

    void fail(ZcodeResponseStatus failStatus, const char *message) {
        if (status.quietEnd) {
            status.hasWrittenTerminator = false;
        }
        status.isComplete = true;
        if (!status.hasWrittenTerminator) {
            writeTerminator(commandSlot->starter);
        }
        commandSlot->failExternal(failStatus);
        out->writeStatus(failStatus);
        if (message != NULL && message[0] != 0) {
            out->writeBigStringField(message);
        }
        if (commandSlot->terminator == EOL_SYMBOL) {
            writeTerminator(EOL_SYMBOL);
        } else {
            status.hasWrittenTerminator = false;
        }
        finish();
    }
    void mildFail(ZcodeResponseStatus failStatus) {
        if (status.quietEnd) {
            status.hasWrittenTerminator = false;
        }
        status.isComplete = true;
        commandSlot->failExternal(failStatus);
    }
    void performFail() {
        if (status.quietEnd) {
            status.hasWrittenTerminator = false;
        }
        status.isComplete = true;
        out->writeStatus((ZcodeResponseStatus) commandSlot->respStatus);
        out->writeBigStringField(commandSlot->failString);
        writeTerminator(EOL_SYMBOL);
        finish();
    }
    void finish() {
        commandSlot->finish();
        status.resetCommand();
    }

    void setComplete() {
        status.isComplete = true;
    }

    void unsetComplete() {
        status.isComplete = false;
    }

    void setNeedsAction() {
        status.needsAction = true;
    }

    ZcodeChannelCommandSlot<ZP>* getInternalSlot() {
        return commandSlot;
    }
};

#endif /* SRC_MAIN_CPP_ZCODE_RUNNING_ZCODERUNNINGCOMMANDSLOT_HPP_ */

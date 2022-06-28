/*
 * ZcodeRunningCommandSlot.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODERUNNINGCOMMANDSLOT_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODERUNNINGCOMMANDSLOT_HPP_
#include "../ZcodeIncludes.hpp"
#include "../parsing/ZcodeChannelCommandSlot.hpp"

struct ZcodeRunningCommandSlotStatus {
    bool isComplete :1;
    bool isStarted :1;
    bool hasWrittenTerminator :1;
    bool hasOutLock :1;
    bool needsAction :1;

    void resetCommand() {
        isComplete = false;
        isStarted = false;
        needsAction = false;
    }
    void fullReset() {
        isComplete = false;
        isStarted = false;
        hasWrittenTerminator = false;
        hasOutLock = false;
        needsAction = false;
    }
};

template<class ZP>
class ZcodeExecutionCommandSlot;

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeRunningCommandSlot {
    friend class ZcodeExecutionCommandSlot<ZP> ;
    ZcodeChannelCommandSlot<ZP> *commandSlot;
    ZcodeOutStream<ZP> *out;
    ZcodeRunningCommandSlotStatus status;
    Zcode<ZP> *zcode;

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

    ZcodeRunningCommandSlot(Zcode<ZP> *zcode, ZcodeOutStream<ZP> *out, ZcodeChannelCommandSlot<ZP> *commandSlot) :
            commandSlot(commandSlot), out(out), status(), zcode(zcode), storedData(0), dataPointer(NULL) {
        status.fullReset();
    }
    uint32_t storedData;
    uint8_t *dataPointer;

    Zcode<ZP>* getZcode() {
        return zcode;
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
        }
    }

    bool isAtSequenceStart() {
        return commandSlot->runStatus.isFirstCommand;
    }
    void startSequence() {
        if (out->isOpen() && out->mostRecent != this) {
            out->close();
        }

        out->mostRecent = this;
        if (!out->isOpen()) {
            out->openResponse(commandSlot->channel);
        }
        if (commandSlot->runStatus.isBroadcast) {
            out->markBroadcast();
        }
    }

    void checkSeqEnd() {
        if (!status.isStarted && commandSlot->runStatus.isFirstCommand) {
            if (!status.hasWrittenTerminator) {
                writeTerminator(EOL_SYMBOL);
            }
            unlock();
            if (out->isOpen() && out->mostRecent == this && (!commandSlot->channel->packetBased || !commandSlot->runStatus.hasData)) {
                //tries to put multiple responses into the same packet
                out->close();
            }
        }
    }

    void fail(ZcodeResponseStatus failStatus, const char *message) {
        status.isComplete = true;
        if (!status.hasWrittenTerminator) {
            writeTerminator(commandSlot->starter);
        }
        commandSlot->failExternal(failStatus);
        out->writeStatus(failStatus);
        out->writeBigStringField(message);
        if (commandSlot->terminator == EOL_SYMBOL) {
            writeTerminator(EOL_SYMBOL);
        } else {
            status.hasWrittenTerminator = false;
        }
        finish();
    }
    void mildFail(ZcodeResponseStatus failStatus) {
        status.isComplete = true;
        commandSlot->failExternal(failStatus);
    }
    void performFail() {
        status.isComplete = true;
        out->writeStatus(commandSlot->respStatus);
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
};
#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODERUNNINGCOMMANDSLOT_HPP_ */

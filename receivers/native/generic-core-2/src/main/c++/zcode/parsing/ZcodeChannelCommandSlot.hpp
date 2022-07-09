/*
 * ZcodeParser.hpp
 *
 *  Created on: 8 Sep 2020
 *      Author: robert
 */

#ifndef SRC_TEST_CPP_ZCODE_PARSING_ZCODECHANNELCOMMANDSLOT_HPP_
#define SRC_TEST_CPP_ZCODE_PARSING_ZCODECHANNELCOMMANDSLOT_HPP_
#include "ZcodeFieldMap.hpp"
#include "ZcodeBigField.hpp"
#include "../ZcodeLockSet.hpp"
#include "../ZcodeIncludes.hpp"

struct ZcodeChannelCommandSlotParsingState {
    bool hasFailed :1;
    bool isComment :1;
    bool isAddressing :1;
    bool isParsingParallel :1;
    bool waitingOnRun :1;
    bool isAtSeqBegining :1;
    bool sequenceDone :1;

    bool isInString :1;
    bool isEscaped :1;
    bool isInBig :1;
    bool isInField :1;

    void resetToCommand() {
        hasFailed = false;
        isComment = false;
        isAddressing = false;
        isParsingParallel = false;
        waitingOnRun = false;
        isAtSeqBegining = false;
        sequenceDone = false;

        isInString = false;
        isEscaped = false;
        isInBig = false;
        isInField = false;
    }
    void resetToSequence() {
        hasFailed = false;
        isComment = false;
        isAddressing = false;
        isParsingParallel = false;
        waitingOnRun = false;
        isAtSeqBegining = true;
        sequenceDone = false;

        isInString = false;
        isEscaped = false;
        isInBig = false;
        isInField = false;
    }

    void resetToFail() {
        hasFailed = true;
        waitingOnRun = true;

        isComment = false;
        isAddressing = false;
        isParsingParallel = false;
        isAtSeqBegining = false;
        sequenceDone = false;

        isInString = false;
        isEscaped = false;
        isInBig = false;
        isInField = false;
    }
};
struct ZcodeChannelCommandSlotStatus {
    bool started :1;
    bool needsMoveAlong :1;
    bool complete :1;

    bool isFirstCommand :1;
    bool isBroadcast :1;
    bool isParallel :1;
    bool hasData :1;

    void resetToCommand() {
        isFirstCommand = false;
        started = false;
        needsMoveAlong = false;
        complete = false;
    }
    void resetToSequence() {
        isFirstCommand = true;
        started = false;
        needsMoveAlong = false;
        complete = false;
        isBroadcast = false;
        isParallel = false;
    }
};

template<class ZP>
class Zcode;

template<class ZP>
class ZcodeRunningCommandSlot;

template<class ZP>
class ZcodeCommandChannel;

template<class ZP>
class ZcodeExecutionCommandSlot;

template<class ZP>
class ZcodeChannelCommandSlot {
    friend class ZcodeRunningCommandSlot<ZP> ;
    friend class ZcodeExecutionCommandSlot<ZP> ;
    ZcodeCommandChannel<ZP> *channel;
    ZcodeFieldMap<ZP> fieldMap;
    ZcodeBigField<ZP> bigField;
    ZcodeLockSet<ZP> lockSet;
    const char *failString;
    ZcodeChannelCommandSlotParsingState state;
    uint16_t respStatus;
    ZcodeChannelCommandSlotStatus runStatus;
    char terminator;
    char starter;

    void resetToCommand() {
        fieldMap.reset();
        bigField.reset();
        state.resetToCommand();
        runStatus.resetToCommand();
    }

    void resetToSequence() {
        fieldMap.reset();
        bigField.reset();
        lockSet.setAll();
        state.resetToSequence();
        runStatus.resetToSequence();
        respStatus = OK;
        failString = NULL;
    }

    bool isWhitespace(char c) {
        return c == ' ' || c == '\r' || c == '\t' || c == ',';
    }

    int8_t getHex(char c) {
        if (c >= 'a') {
            return (int8_t) (c <= 'f' ? c - 'a' + 10 : -1);
        } else {
            return (int8_t) (c >= '0' && c <= '9' ? c - '0' : -1);
        }
    }

    void parseFail(char c, ZcodeResponseStatus status, const char *message) {
        state.resetToFail();
        respStatus = status;
        failString = message;

        terminator = c;
    }
    void failExternal(ZcodeResponseStatus status) {
        state.resetToFail();
        state.waitingOnRun = false;
        respStatus = status;
        failString = NULL;
    }

    void inStringParse(char c) {
        if (c == BIGFIELD_QUOTE_MARKER) {
            state.isInString = false;
            if (state.isEscaped || bigField.isInNibble()) {
                parseFail(c, PARSE_ERROR, ZCODE_STRING_SURROUND("Misplaced string field end"));
                return;
            }
            return;
        }
        if (c == ANDTHEN_SYMBOL || c == ORELSE_SYMBOL || c == EOL_SYMBOL) {
            parseFail(c, PARSE_ERROR, ZCODE_STRING_SURROUND("String field not ended"));
            return;
        }
        if (bigField.isAtEnd()) {
            parseFail(c, TOO_BIG, ZCODE_STRING_SURROUND("String field too long"));
            return;
        }
        if (state.isEscaped) {
            int8_t val = getHex(c);
            if (val == -1) {
                parseFail(c, PARSE_ERROR, ZCODE_STRING_SURROUND("String escaping error"));
                return;
            }
            if (bigField.isInNibble()) {
                state.isEscaped = false;
            }
            bigField.addNibble(val);
        } else if (c == STRING_ESCAPE_SYMBOL) {
            state.isEscaped = true;
        } else {
            bigField.addByte(c);
        }
    }

    void inBigParse(char c) {
        int8_t val = getHex(c);
        if (val == -1) {
            state.isInBig = false;
            if (bigField.isInNibble()) {
                parseFail(c, PARSE_ERROR, ZCODE_STRING_SURROUND("Big field odd length"));
                return;
            }
            outerCommandParse(c);
            return;
        }
        if (!bigField.addNibble(val)) {
            parseFail(c, TOO_BIG, ZCODE_STRING_SURROUND("Big field too long"));
            return;
        }
    }

    void inFieldParse(char c) {
        int8_t val = getHex(c);
        if (val == -1) {
            state.isInField = false;
            outerCommandParse(c);
            return;
        }
        if (!fieldMap.add4(val)) {
            parseFail(c, TOO_BIG, ZCODE_STRING_SURROUND("Field too long"));
            return;
        }
    }

    void outerCommandParse(char c) {
        if (c >= 'A' && c <= 'Z') {
            state.isInField = true;
            if (!fieldMap.addBlank(c)) {
                parseFail(c, PARSE_ERROR, ZCODE_STRING_SURROUND("Field repeated"));
            }
        } else if (c == BIGFIELD_PREFIX_MARKER) {
            state.isInBig = true;
        } else if (c == BIGFIELD_QUOTE_MARKER) {
            state.isInString = true;
        } else if (c == ANDTHEN_SYMBOL || c == ORELSE_SYMBOL || c == EOL_SYMBOL) {
            state.waitingOnRun = true;
            terminator = c;
        } else {
            parseFail(c, PARSE_ERROR, ZCODE_STRING_SURROUND("Unknown character"));
        }
    }

    void lockingParse(char c) {
        int8_t val = getHex(c);
        if (val == -1) {
            state.isParsingParallel = false;
            acceptByte(c);
            return;
        }
        if (!lockSet.setLocks4(val)) {
            parseFail(c, TOO_BIG, ZCODE_STRING_SURROUND("Too many locks"));
        }
    }

    void sequenceBeginingParse(char c) {
        if (c == BROADCAST_PREFIX) {
            if (runStatus.isBroadcast) {
                parseFail(c, PARSE_ERROR, ZCODE_STRING_SURROUND("Broadcast set twice"));
            }
            runStatus.isBroadcast = true;
        } else if (c == DEBUG_PREFIX) {
            state.isComment = true;
        } else if (c == ADDRESS_PREFIX) {
            state.isAddressing = true;
            fieldMap.add16('Z', 0x0f);
        } else if (c == PARALLEL_PREFIX) {
            if (runStatus.isParallel) {
                parseFail(c, PARSE_ERROR, ZCODE_STRING_SURROUND("Locks set twice"));
            } else {
                lockSet.clearAll();
                state.isParsingParallel = true;
                runStatus.isParallel = true;
            }
        } else {
            state.isAtSeqBegining = false;
            acceptByte(c);
        }
    }

    void acceptEdgeCase(char c) {
        if (state.waitingOnRun) {
            // left blank, just drops out.
        } else if (state.sequenceDone || state.isComment || (state.hasFailed && (c != ORELSE_SYMBOL || respStatus < CMD_FAIL))) {
            if (c == EOL_SYMBOL) {
                resetToSequence();
                terminator = EOL_SYMBOL;
                starter = terminator;
            }
        } else if (state.hasFailed && c == ORELSE_SYMBOL && respStatus >= CMD_FAIL) {
            resetToCommand();
            starter = ORELSE_SYMBOL;
        } else if (state.isParsingParallel) {
            if (!isWhitespace(c)) {
                lockingParse(c);
            }
        } else if (state.isAtSeqBegining) {
            if (!isWhitespace(c)) {
                sequenceBeginingParse(c);
            }
        } else if (state.isAddressing) {
            if (c == EOL_SYMBOL) {
                state.waitingOnRun = true;
                terminator = c;
            } else {
                bigField.addByte(c);
            }
        } else {
            parseFail(c, UNKNOWN_ERROR, ZCODE_STRING_SURROUND("Internal error"));
        }
    }

public:
    ZcodeChannelCommandSlot(Zcode<ZP> *zcode, ZcodeBigField<ZP> bigField, ZcodeCommandChannel<ZP> *channel) :
            channel(channel), fieldMap(), bigField(bigField), lockSet(zcode->getLocks()), state(), respStatus(OK), runStatus(), terminator('\n'), starter('\n') {
        resetToSequence();
    }

    bool acceptByte(char c) { // returns false <=> the data couldn't be used and needs to be resent later.
        bool wasWaiting = state.waitingOnRun;
        runStatus.hasData = true;
        if (state.hasFailed || state.isComment || state.isAddressing || state.isParsingParallel || state.waitingOnRun || state.isAtSeqBegining
                || state.sequenceDone) {
            acceptEdgeCase(c);
        } else {
            if (state.isInString) {
                inStringParse(c);
            } else if (isWhitespace(c)) {
                // drops out.
            } else if (state.isInBig) {
                inBigParse(c);
            } else if (state.isInField) {
                inFieldParse(c);
            } else {
                outerCommandParse(c);
            }
        }
        return !wasWaiting; // waiting on run is the only circumstance where we don't want data.
    }

    void finish() {
        state.waitingOnRun = false;
        if (!state.hasFailed) {
            if (terminator == EOL_SYMBOL) {
                resetToSequence();
            } else if (terminator == ANDTHEN_SYMBOL) {
                resetToCommand();
            } else if (terminator == ORELSE_SYMBOL) {
                state.sequenceDone = true;
            }
            starter = terminator;
        } else {
            if (terminator == EOL_SYMBOL) {
                resetToSequence();
                starter = terminator;
            } else if (terminator == ORELSE_SYMBOL) {
                if (respStatus >= CMD_FAIL) {
                    resetToCommand();
                    starter = terminator;
                }
            }
        }
    }
    void clearHasData() {
        runStatus.hasData = false;
    }
    ZcodeCommandChannel<ZP>* getCommandChannel() {
        return channel;
    }
};
#endif /* SRC_TEST_CPP_ZCODE_PARSING_ZCODECHANNELCOMMANDSLOT_HPP_ */
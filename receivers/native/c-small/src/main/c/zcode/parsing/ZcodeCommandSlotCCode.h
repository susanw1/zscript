/*
 * ZcodeCommandSlotCCode.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODECOMMANDSLOTCCODE_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODECOMMANDSLOTCCODE_H_
#include "ZcodeCommandSlot.h"
#include "ZcodeFieldMapCCode.h"
#include "ZcodeBigFieldCCode.h"

void ZcodeCommandSlot_outerCommandParse(char c);

void ZcodeCommandSlotParsingState_resetToCommand(ZcodeCommandSlotParsingState *state) {
    state->hasFailed = false;
    state->isComment = false;
    state->isParsingParallel = false;
    state->waitingOnRun = false;
    state->isAtSeqBegining = false;
    state->sequenceDone = false;

    state->isInString = false;
    state->isEscaped = false;
    state->isInBig = false;
    state->isInField = false;
}
void ZcodeCommandSlotParsingState_resetToSequence(ZcodeCommandSlotParsingState *state) {
    state->hasFailed = false;
    state->isComment = false;
    state->isParsingParallel = false;
    state->waitingOnRun = false;
    state->isAtSeqBegining = true;
    state->sequenceDone = false;

    state->isInString = false;
    state->isEscaped = false;
    state->isInBig = false;
    state->isInField = false;
}

void ZcodeCommandSlotParsingState_resetToFail(ZcodeCommandSlotParsingState *state) {
    state->hasFailed = true;
    state->waitingOnRun = true;

    state->isComment = false;
    state->isParsingParallel = false;
    state->isAtSeqBegining = false;
    state->sequenceDone = false;

    state->isInString = false;
    state->isEscaped = false;
    state->isInBig = false;
    state->isInField = false;
}

void ZcodeCommandSlotStatus_reset(ZcodeCommandSlotStatus *state) {
    state->isFirstCommand = true;
    state->isBroadcast = false;
}
void ZcodeInitialiseSlot() {
    ZcodeCommandSlot *slot = &zcode.slot;
    ZcodeBigFieldReset();
    ZcodeFieldMapReset();
    ZcodeCommandSlotParsingState_resetToSequence(&slot->state);
    ZcodeCommandSlotStatus_reset(&slot->runStatus);
    slot->runStatus.hasWrittenTerminator = true;
    slot->runStatus.quietEnd = false;
    slot->runStatus.hasOutLock = false;
    slot->runStatus.hasData = false;
    slot->respStatus = OK;
    slot->terminator = '\n';
    slot->starter = '\n';
}

void ZcodeCommandSlot_resetToCommand() {
    ZcodeCommandSlot *slot = &zcode.slot;
    ZcodeFieldMapReset();
    ZcodeBigFieldReset();
    ZcodeCommandSlotParsingState_resetToCommand(&slot->state);
    slot->runStatus.isFirstCommand = false;
}

void ZcodeCommandSlot_resetToSequence() {
    ZcodeCommandSlot *slot = &zcode.slot;
    ZcodeFieldMapReset();
    ZcodeBigFieldReset();
    ZcodeCommandSlotParsingState_resetToSequence(&slot->state);
    ZcodeCommandSlotStatus_reset(&slot->runStatus);
    slot->respStatus = OK;
}

bool ZcodeCommandSlot_isWhitespace(char c) {
    return c == ' ' || c == '\r' || c == '\t' || c == ',';
}

int8_t ZcodeCommandSlot_getHex(char c) {
    if (c >= 'a') {
        return (int8_t) (c <= 'f' ? (int8_t) (c - 'a' + 10) : -1);
    } else {
        return (int8_t) (c >= '0' && c <= '9' ? (int8_t) (c - '0') : -1);
    }
}

void ZcodeCommandSlot_parseFail(char c, ZcodeResponseStatus status) {
    ZcodeCommandSlot *slot = &zcode.slot;
    ZcodeCommandSlotParsingState_resetToFail(&slot->state);
    slot->respStatus = status;

    slot->terminator = c;
}
void ZcodeCommandSlot_failExternal(ZcodeResponseStatus status) {
    ZcodeCommandSlot *slot = &zcode.slot;
    ZcodeCommandSlotParsingState_resetToFail(&slot->state);
    slot->state.waitingOnRun = false;
    slot->respStatus = status;
}

void ZcodeCommandSlot_inStringParse(char c) {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (c == BIGFIELD_QUOTE_MARKER) {
        slot->state.isInString = false;
        if (slot->state.isEscaped || slot->bigField.inNibble) {
            ZcodeCommandSlot_parseFail(c, PARSE_ERROR);
            return;
        }
        return;
    }
    if (c == EOL_SYMBOL) {
        ZcodeCommandSlot_parseFail(c, PARSE_ERROR);
        return;
    }
    if (slot->bigField.pos == ZCODE_BIG_FIELD_SIZE) {
        ZcodeCommandSlot_parseFail(c, TOO_BIG);
        return;
    }
    if (slot->state.isEscaped) {
        int8_t val = ZcodeCommandSlot_getHex(c);
        if (val == -1) {
            ZcodeCommandSlot_parseFail(c, PARSE_ERROR);
            return;
        }
        if (slot->bigField.inNibble) {
            slot->state.isEscaped = false;
        }
        ZcodeBigFieldAddNibble((uint8_t) val);
    } else if (c == STRING_ESCAPE_SYMBOL) {
        slot->state.isEscaped = true;
    } else {
        ZcodeBigFieldAddByte((uint8_t) c);
    }
}

void ZcodeCommandSlot_inBigParse(char c) {
    ZcodeCommandSlot *slot = &zcode.slot;
    int8_t val = ZcodeCommandSlot_getHex(c);
    if (val == -1) {
        slot->state.isInBig = false;
        if (slot->bigField.inNibble) {
            ZcodeCommandSlot_parseFail(c, PARSE_ERROR);
            return;
        }
        ZcodeCommandSlot_outerCommandParse(c);
        return;
    }
    if (!ZcodeBigFieldAddNibble((uint8_t) val)) {
        ZcodeCommandSlot_parseFail(c, TOO_BIG);
        return;
    }
}

void ZcodeCommandSlot_inFieldParse(char c) {
    ZcodeCommandSlot *slot = &zcode.slot;
    int8_t val = ZcodeCommandSlot_getHex(c);
    if (val == -1) {
        slot->state.isInField = false;
        ZcodeCommandSlot_outerCommandParse(c);
        return;
    }
    if (!ZcodeFieldMapAdd4((uint8_t) val)) {
        ZcodeCommandSlot_parseFail(c, TOO_BIG);
        return;
    }
}

void ZcodeCommandSlot_outerCommandParse(char c) {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (c >= 'A' && c <= 'Z') {
        slot->state.isInField = true;
        if (!ZcodeFieldMapAddBlank(c)) {
            ZcodeCommandSlot_parseFail(c, PARSE_ERROR);
        }
    } else if (c == BIGFIELD_PREFIX_MARKER) {
        slot->state.isInBig = true;
    } else if (c == BIGFIELD_QUOTE_MARKER) {
        slot->state.isInString = true;
    } else if (c == ANDTHEN_SYMBOL || c == ORELSE_SYMBOL || c == EOL_SYMBOL) {
        slot->state.waitingOnRun = true;
        slot->terminator = c;
    } else {
        ZcodeCommandSlot_parseFail(c, PARSE_ERROR);
    }
}

void ZcodeSlotAcceptByteLoopback(char c) {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (slot->state.isInString) {
        ZcodeCommandSlot_inStringParse(c);
    } else if (ZcodeCommandSlot_isWhitespace(c)) {
        // drops out.
    } else if (slot->state.isInBig) {
        ZcodeCommandSlot_inBigParse(c);
    } else if (slot->state.isInField) {
        ZcodeCommandSlot_inFieldParse(c);
    } else {
        ZcodeCommandSlot_outerCommandParse(c);
    }
}
void ZcodeCommandSlot_sequenceBeginingParse(char c) {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (c == BROADCAST_PREFIX) {
        if (slot->runStatus.isBroadcast) {
            ZcodeCommandSlot_parseFail(c, PARSE_ERROR);
        }
        slot->runStatus.isBroadcast = true;
    } else if (c == DEBUG_PREFIX) {
        slot->state.isComment = true;
    } else if (c == ADDRESS_PREFIX) {
        slot->state.isAtSeqBegining = false;
        ZcodeCommandSlot_parseFail(c, BAD_ADDRESSING);
    } else if (c == PARALLEL_PREFIX) {
        slot->state.isParsingParallel = true;
    } else {
        slot->state.isAtSeqBegining = false;
        ZcodeSlotAcceptByteLoopback(c);
    }
}

void ZcodeCommandSlot_acceptEdgeCaseRemaining(char c) {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (slot->state.isAtSeqBegining) {
        if (!ZcodeCommandSlot_isWhitespace(c)) {
            ZcodeCommandSlot_sequenceBeginingParse(c);
        }
    } else {
        ZcodeCommandSlot_parseFail(c, UNKNOWN_ERROR);
    }
}
void ZcodeCommandSlot_acceptEdgeCase(char c) {
    ZcodeCommandSlot *slot = &zcode.slot;
    if (slot->state.waitingOnRun) {
        // left blank, just drops out.
    } else if (slot->state.sequenceDone || slot->state.isComment || (slot->state.hasFailed && (c != ORELSE_SYMBOL || slot->respStatus < CMD_FAIL))) {
        if (c == EOL_SYMBOL) {
            ZcodeCommandSlot_resetToSequence();
            slot->terminator = EOL_SYMBOL;
            slot->starter = slot->terminator;
        }
    } else if (slot->state.hasFailed && c == ORELSE_SYMBOL && slot->respStatus >= CMD_FAIL) {
        ZcodeCommandSlot_resetToCommand();
        slot->starter = ORELSE_SYMBOL;
    } else if (slot->state.isParsingParallel) {
        if (!ZcodeCommandSlot_isWhitespace(c)) {
            int8_t val = ZcodeCommandSlot_getHex(c);
            if (val == -1) {
                slot->state.isParsingParallel = false;
                if (slot->state.hasFailed || slot->state.isComment || slot->state.isParsingParallel || slot->state.waitingOnRun
                        || slot->state.isAtSeqBegining
                        || slot->state.sequenceDone) {
                    ZcodeCommandSlot_acceptEdgeCaseRemaining(c);
                } else {
                    ZcodeSlotAcceptByteLoopback(c);
                }
            }
            // Ignore all lock setting, as we don't do parallelism anyway.
        }
    } else {
        ZcodeCommandSlot_acceptEdgeCaseRemaining(c);
    }
}

bool ZcodeSlotAcceptByte(char c) { // returns false <=> the data couldn't be used and needs to be resent later.
    ZcodeCommandSlot *slot = &zcode.slot;
    bool wasWaiting = slot->state.waitingOnRun;
    slot->runStatus.hasData = true;
    if (slot->state.hasFailed || slot->state.isComment || slot->state.isParsingParallel || slot->state.waitingOnRun || slot->state.isAtSeqBegining
            || slot->state.sequenceDone) {
        ZcodeCommandSlot_acceptEdgeCase(c);
    } else {
        ZcodeSlotAcceptByteLoopback(c);
    }
    return !wasWaiting; // waiting on run is the only circumstance where we don't want data.
}

void ZcodeCommandSlot_finish() {
    ZcodeCommandSlot *slot = &zcode.slot;
    slot->state.waitingOnRun = false;
    if (!slot->state.hasFailed) {
        if (slot->terminator == EOL_SYMBOL) {
            ZcodeCommandSlot_resetToSequence();
        } else if (slot->terminator == ANDTHEN_SYMBOL) {
            ZcodeCommandSlot_resetToCommand();
        } else if (slot->terminator == ORELSE_SYMBOL) {
            slot->state.sequenceDone = true;
        }
        slot->starter = slot->terminator;
    } else {
        if (slot->terminator == EOL_SYMBOL) {
            ZcodeCommandSlot_resetToSequence();
            slot->starter = slot->terminator;
        } else if (slot->terminator == ORELSE_SYMBOL) {
            if (slot->respStatus >= CMD_FAIL) {
                ZcodeCommandSlot_resetToCommand();
                slot->starter = slot->terminator;
            }
        }
    }
}
#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODECOMMANDSLOTCCODE_H_ */

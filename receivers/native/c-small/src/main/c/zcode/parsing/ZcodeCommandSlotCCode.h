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

void ZcodeCommandSlot_outerCommandParse(ZcodeCommandSlot *slot, char c);

void ZcodeCommandSlotParsingState_resetToCommand(ZcodeCommandSlotParsingState *state) {
    state->hasFailed = false;
    state->isComment = false;
    state->isAddressing = false;
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
    state->isAddressing = false;
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
    state->isAddressing = false;
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
void ZcodeInitialiseSlot(ZcodeCommandSlot *slot) {
    ZcodeBigFieldReset(&slot->bigField);
    ZcodeFieldMapReset(&slot->fieldMap);
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

void ZcodeCommandSlot_resetToCommand(ZcodeCommandSlot *slot) {
    ZcodeFieldMapReset(&slot->fieldMap);
    ZcodeBigFieldReset(&slot->bigField);
    ZcodeCommandSlotParsingState_resetToCommand(&slot->state);
    slot->runStatus.isFirstCommand = false;
}

void ZcodeCommandSlot_resetToSequence(ZcodeCommandSlot *slot) {
    ZcodeFieldMapReset(&slot->fieldMap);
    ZcodeBigFieldReset(&slot->bigField);
    ZcodeCommandSlotParsingState_resetToSequence(&slot->state);
    ZcodeCommandSlotStatus_reset(&slot->runStatus);
    slot->respStatus = OK;
}

bool ZcodeCommandSlot_isWhitespace(char c) {
    return c == ' ' || c == '\r' || c == '\t' || c == ',';
}

int8_t ZcodeCommandSlot_getHex(char c) {
    if (c >= 'a') {
        return (int8_t) (c <= 'f' ? c - 'a' + 10 : -1);
    } else {
        return (int8_t) (c >= '0' && c <= '9' ? c - '0' : -1);
    }
}

void ZcodeCommandSlot_parseFail(ZcodeCommandSlot *slot, char c, ZcodeResponseStatus status) {
    ZcodeCommandSlotParsingState_resetToFail(&slot->state);
    slot->respStatus = status;

    slot->terminator = c;
}
void ZcodeCommandSlot_failExternal(ZcodeCommandSlot *slot, ZcodeResponseStatus status) {
    ZcodeCommandSlotParsingState_resetToFail(&slot->state);
    slot->state.waitingOnRun = false;
    slot->respStatus = status;
}

void ZcodeCommandSlot_inStringParse(ZcodeCommandSlot *slot, char c) {
    if (c == BIGFIELD_QUOTE_MARKER) {
        slot->state.isInString = false;
        if (slot->state.isEscaped || slot->bigField.inNibble) {
            ZcodeCommandSlot_parseFail(slot, c, PARSE_ERROR);
            return;
        }
        return;
    }
    if (c == EOL_SYMBOL) {
        ZcodeCommandSlot_parseFail(slot, c, PARSE_ERROR);
        return;
    }
    if (slot->bigField.pos == ZCODE_BIG_FIELD_SIZE) {
        ZcodeCommandSlot_parseFail(slot, c, TOO_BIG);
        return;
    }
    if (slot->state.isEscaped) {
        int8_t val = ZcodeCommandSlot_getHex(c);
        if (val == -1) {
            ZcodeCommandSlot_parseFail(slot, c, PARSE_ERROR);
            return;
        }
        if (slot->bigField.inNibble) {
            slot->state.isEscaped = false;
        }
        ZcodeBigFieldAddNibble(&slot->bigField, (uint8_t) val);
    } else if (c == STRING_ESCAPE_SYMBOL) {
        slot->state.isEscaped = true;
    } else {
        ZcodeBigFieldAddByte(&slot->bigField, (uint8_t) c);
    }
}

void ZcodeCommandSlot_inBigParse(ZcodeCommandSlot *slot, char c) {
    int8_t val = ZcodeCommandSlot_getHex(c);
    if (val == -1) {
        slot->state.isInBig = false;
        if (slot->bigField.inNibble) {
            ZcodeCommandSlot_parseFail(slot, c, PARSE_ERROR);
            return;
        }
        ZcodeCommandSlot_outerCommandParse(slot, c);
        return;
    }
    if (!ZcodeBigFieldAddNibble(&slot->bigField, (uint8_t) val)) {
        ZcodeCommandSlot_parseFail(slot, c, TOO_BIG);
        return;
    }
}

void ZcodeCommandSlot_inFieldParse(ZcodeCommandSlot *slot, char c) {
    int8_t val = ZcodeCommandSlot_getHex(c);
    if (val == -1) {
        slot->state.isInField = false;
        ZcodeCommandSlot_outerCommandParse(slot, c);
        return;
    }
    if (!ZcodeFieldMapAdd4(&slot->fieldMap, (uint8_t) val)) {
        ZcodeCommandSlot_parseFail(slot, c, TOO_BIG);
        return;
    }
}

void ZcodeCommandSlot_outerCommandParse(ZcodeCommandSlot *slot, char c) {
    if (c >= 'A' && c <= 'Z') {
        slot->state.isInField = true;
        if (!ZcodeFieldMapAddBlank(&slot->fieldMap, c)) {
            ZcodeCommandSlot_parseFail(slot, c, PARSE_ERROR);
        }
    } else if (c == BIGFIELD_PREFIX_MARKER) {
        slot->state.isInBig = true;
    } else if (c == BIGFIELD_QUOTE_MARKER) {
        slot->state.isInString = true;
    } else if (c == ANDTHEN_SYMBOL || c == ORELSE_SYMBOL || c == EOL_SYMBOL) {
        slot->state.waitingOnRun = true;
        slot->terminator = c;
    } else {
        ZcodeCommandSlot_parseFail(slot, c, PARSE_ERROR);
    }
}

void ZcodeCommandSlot_sequenceBeginingParse(ZcodeCommandSlot *slot, char c) {
    if (c == BROADCAST_PREFIX) {
        if (slot->runStatus.isBroadcast) {
            ZcodeCommandSlot_parseFail(slot, c, PARSE_ERROR);
        }
        slot->runStatus.isBroadcast = true;
    } else if (c == DEBUG_PREFIX) {
        slot->state.isComment = true;
    } else if (c == ADDRESS_PREFIX) {
        slot->state.isAtSeqBegining = false;
        slot->state.isAddressing = true;
        ZcodeFieldMapAdd16(&slot->fieldMap, 'Z', 0x11);
    } else if (c == PARALLEL_PREFIX) {
        slot->state.isParsingParallel = true;
    } else {
        slot->state.isAtSeqBegining = false;
        ZcodeSlotAcceptByte(slot, c);
    }
}

void ZcodeCommandSlot_acceptEdgeCase(ZcodeCommandSlot *slot, char c) {
    if (slot->state.waitingOnRun) {
        // left blank, just drops out.
    } else if (slot->state.sequenceDone || slot->state.isComment || (slot->state.hasFailed && (c != ORELSE_SYMBOL || slot->respStatus < CMD_FAIL))) {
        if (c == EOL_SYMBOL) {
            ZcodeCommandSlot_resetToSequence(slot);
            slot->terminator = EOL_SYMBOL;
            slot->starter = slot->terminator;
        }
    } else if (slot->state.hasFailed && c == ORELSE_SYMBOL && slot->respStatus >= CMD_FAIL) {
        ZcodeCommandSlot_resetToCommand(slot);
        slot->starter = ORELSE_SYMBOL;
    } else if (slot->state.isParsingParallel) {
        if (!ZcodeCommandSlot_isWhitespace(c)) {
            int8_t val = ZcodeCommandSlot_getHex(c);
            if (val == -1) {
                slot->state.isParsingParallel = false;
                ZcodeSlotAcceptByte(slot, c);
            }
            // Ignore all lock setting, as we don't do parallelism anyway.
        }
    } else if (slot->state.isAtSeqBegining) {
        if (!ZcodeCommandSlot_isWhitespace(c)) {
            ZcodeCommandSlot_sequenceBeginingParse(slot, c);
        }
    } else if (slot->state.isAddressing) {
        ZcodeBigFieldAddByte(&slot->bigField, (uint8_t) c);
        if (c == EOL_SYMBOL) {
            slot->state.waitingOnRun = true;
            slot->terminator = c;
        }
    } else {
        ZcodeCommandSlot_parseFail(slot, c, UNKNOWN_ERROR);
    }
}

bool ZcodeSlotAcceptByte(ZcodeCommandSlot *slot, char c) { // returns false <=> the data couldn't be used and needs to be resent later.
    bool wasWaiting = slot->state.waitingOnRun;
    slot->runStatus.hasData = true;
    if (slot->state.hasFailed || slot->state.isComment || slot->state.isAddressing || slot->state.isParsingParallel || slot->state.waitingOnRun || slot->state.isAtSeqBegining
            || slot->state.sequenceDone) {
        ZcodeCommandSlot_acceptEdgeCase(slot, c);
    } else {
        if (slot->state.isInString) {
            ZcodeCommandSlot_inStringParse(slot, c);
        } else if (ZcodeCommandSlot_isWhitespace(c)) {
            // drops out.
        } else if (slot->state.isInBig) {
            ZcodeCommandSlot_inBigParse(slot, c);
        } else if (slot->state.isInField) {
            ZcodeCommandSlot_inFieldParse(slot, c);
        } else {
            ZcodeCommandSlot_outerCommandParse(slot, c);
        }
    }
    return !wasWaiting; // waiting on run is the only circumstance where we don't want data.
}

void ZcodeCommandSlot_finish(ZcodeCommandSlot *slot) {
    slot->state.waitingOnRun = false;
    if (!slot->state.hasFailed) {
        if (slot->terminator == EOL_SYMBOL) {
            ZcodeCommandSlot_resetToSequence(slot);
        } else if (slot->terminator == ANDTHEN_SYMBOL) {
            ZcodeCommandSlot_resetToCommand(slot);
        } else if (slot->terminator == ORELSE_SYMBOL) {
            slot->state.sequenceDone = true;
        }
        slot->starter = slot->terminator;
    } else {
        if (slot->terminator == EOL_SYMBOL) {
            ZcodeCommandSlot_resetToSequence(slot);
            slot->starter = slot->terminator;
        } else if (slot->terminator == ORELSE_SYMBOL) {
            if (slot->respStatus >= CMD_FAIL) {
                ZcodeCommandSlot_resetToCommand(slot);
                slot->starter = slot->terminator;
            }
        }
    }
}
#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODECOMMANDSLOTCCODE_H_ */

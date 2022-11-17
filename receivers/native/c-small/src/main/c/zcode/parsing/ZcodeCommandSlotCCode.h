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

void ZcodeCommandSlotParsingState__resetToCommand(ZcodeCommandSlotParsingState *state) {
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
void ZcodeCommandSlotParsingState__resetToSequence(ZcodeCommandSlotParsingState *state) {
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

void ZcodeCommandSlotParsingState__resetToFail(ZcodeCommandSlotParsingState *state) {
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

void ZcodeCommandSlotStatus__reset(ZcodeCommandSlotStatus *state) {
    state->isFirstCommand = true;
    state->isBroadcast = false;
    state->isParallel = false;
}

void ZcodeCommandSlot__resetToCommand(ZcodeCommandSlot *slot) {
    ZcodeFieldMapReset(&slot->fieldMap);
    ZcodeBigFieldReset(&slot->bigField);
    ZcodeCommandSlotParsingState__resetToCommand(&slot->state);
    &slot->runStatus->isFirstCommand = false;
}

void ZcodeCommandSlot__resetToSequence(ZcodeCommandSlot *slot) {
    ZcodeFieldMapReset(&slot->fieldMap);
    ZcodeBigFieldReset(&slot->bigField);
    ZcodeCommandSlotParsingState__resetToSequence(&slot->state);
    ZcodeCommandSlotStatus__reset(&slot->runStatus);
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
    ZcodeCommandSlotParsingState__resetToFail(&slot->state);
    slot->respStatus = status;

    slot->terminator = c;
}
void ZcodeCommandSlot_failExternal(ZcodeCommandSlot *slot, ZcodeResponseStatus status) {
    ZcodeCommandSlotParsingState__resetToFail(&slot->state);
    slot->state.waitingOnRun = false;
    slot->respStatus = status;
}

void ZcodeCommandSlot_inStringParse(ZcodeCommandSlot *slot, char c) {
    if (c == BIGFIELD_QUOTE_MARKER) {
        slot->state.isInString = false;
        if (slot->state.isEscaped || slot->bigField.inNibble) {
            ZcodeCommandSlot__parseFail(c, PARSE_ERROR);
            return;
        }
        return;
    }
    if (c == EOL_SYMBOL) {
        ZcodeCommandSlot__parseFail(c, PARSE_ERROR);
        return;
    }
    if (slot->bigField.pos == ZCODE_BIG_FIELD_SIZE) {
        ZcodeCommandSlot__parseFail(c, TOO_BIG);
        return;
    }
    if (slot->state.isEscaped) {
        int8_t val = ZcodeCommandSlot__getHex(c);
        if (val == -1) {
            ZcodeCommandSlot__parseFail(c, PARSE_ERROR);
            return;
        }
        if (slot->bigField.inNibble) {
            slot->state.isEscaped = false;
        }
        ZcodeBigFieldAddNibble(slot->bigField, val);
    } else if (c == STRING_ESCAPE_SYMBOL) {
        slot->state.isEscaped = true;
    } else {
        ZcodeBigFieldAddByte(slot->bigField, c);
    }
}

void ZcodeCommandSlot_inBigParse(ZcodeCommandSlot *slot, char c) {
    int8_t val = ZcodeCommandSlot__getHex(c);
    if (val == -1) {
        slot->state.isInBig = false;
        if (slot->bigField.inNibble) {
            ZcodeCommandSlot__parseFail(c, PARSE_ERROR);
            return;
        }
        ZcodeCommandSlot__outerCommandParse(c);
        return;
    }
    if (!ZcodeBigFieldAddNibble(slot->bigField, val)) {
        ZcodeCommandSlot__parseFail(c, TOO_BIG);
        return;
    }
}

void ZcodeCommandSlot_inFieldParse(ZcodeCommandSlot *slot, char c) {
    int8_t val = ZcodeCommandSlot__getHex(c);
    if (val == -1) {
        slot->state.isInField = false;
        ZcodeCommandSlot__outerCommandParse(c);
        return;
    }
    if (!ZcodeFieldMapAdd4(slot->fieldMap, val)) {
        ZcodeCommandSlot__parseFail(c, TOO_BIG);
        return;
    }
}

void ZcodeCommandSlot_outerCommandParse(ZcodeCommandSlot *slot, char c) {
    if (c >= 'A' && c <= 'Z') {
        slot->state.isInField = true;
        if (!ZcodeFieldMapAddBlank(slot->fieldMap, c)) {
            ZcodeCommandSlot__parseFail(c, PARSE_ERROR);
        }
    } else if (c == BIGFIELD_PREFIX_MARKER) {
        slot->state.isInBig = true;
    } else if (c == BIGFIELD_QUOTE_MARKER) {
        slot->state.isInString = true;
    } else if (c == ANDTHEN_SYMBOL || c == ORELSE_SYMBOL || c == EOL_SYMBOL) {
        slot->state.waitingOnRun = true;
        slot->terminator = c;
    } else {
        ZcodeCommandSlot__parseFail(c, PARSE_ERROR);
    }
}

void ZcodeCommandSlot_sequenceBeginingParse(ZcodeCommandSlot *slot, char c) {
    if (c == BROADCAST_PREFIX) {
        if (slot->runStatus.isBroadcast) {
            ZcodeCommandSlot__parseFail(c, PARSE_ERROR);
        }
        slot->runStatus.isBroadcast = true;
    } else if (c == DEBUG_PREFIX) {
        slot->state.isComment = true;
    } else if (c == ADDRESS_PREFIX) {
        slot->state.isAtSeqBegining = false;
        slot->state.isAddressing = true;
        ZcodeFieldMapAdd16(&slot->fieldMap, 'Z', 0x11);
    } else if (c == PARALLEL_PREFIX) {
        if (slot->runStatus.isParallel) {
            ZcodeCommandSlot__parseFail(c, PARSE_ERROR);
        } else {
            slot->state.isParsingParallel = true;
            slot->runStatus.isParallel = true;
        }
    } else {
        slot->state.isAtSeqBegining = false;
        ZcodeCommandSlot__acceptByte(c);
    }
}

void ZcodeCommandSlot_acceptEdgeCase(ZcodeCommandSlot *slot, char c) {
    if (slot->state.waitingOnRun) {
        // left blank, just drops out.
    } else if (slot->state.sequenceDone || slot->state.isComment || (slot->state.hasFailed && (c != ORELSE_SYMBOL || slot->respStatus < CMD_FAIL))) {
        if (c == EOL_SYMBOL) {
            ZcodeCommandSlot__resetToSequence();
            slot->terminator = EOL_SYMBOL;
            slot->starter = slot->terminator;
        }
    } else if (slot->state.hasFailed && c == ORELSE_SYMBOL && slot->respStatus >= CMD_FAIL) {
        ZcodeCommandSlot__resetToCommand();
        slot->starter = ORELSE_SYMBOL;
    } else if (slot->state.isParsingParallel) {
        if (!ZcodeCommandSlot__isWhitespace(c)) {
            int8_t val = ZcodeCommandSlot__getHex(c);
            if (val == -1) {
                slot->state.isParsingParallel = false;
                ZcodeCommandSlot__acceptByte(c);
            }
            // Ignore all lock setting, as we don't do parallelism anyway.
        }
    } else if (slot->state.isAtSeqBegining) {
        if (!ZcodeCommandSlot__isWhitespace(c)) {
            ZcodeCommandSlot__sequenceBeginingParse(c);
        }
    } else if (slot->state.isAddressing) {
        ZcodeBigFieldAddByte(slot->bigField, c);
        if (c == EOL_SYMBOL) {
            slot->state.waitingOnRun = true;
            slot->terminator = c;
        }
    } else {
        ZcodeCommandSlot__parseFail(c, UNKNOWN_ERROR);
    }
}

bool ZcodeAcceptByte(ZcodeCommandSlot *slot, char c) { // returns false <=> the data couldn't be used and needs to be resent later.
    bool wasWaiting = slot->state.waitingOnRun;
    slot->runStatus.hasData = true;
    if (slot->state.hasFailed || slot->state.isComment || slot->state.isAddressing || slot->state.isParsingParallel || slot->state.waitingOnRun || slot->state.isAtSeqBegining
            || slot->state.sequenceDone) {
        ZcodeCommandSlot__acceptEdgeCase(c);
    } else {
        if (slot->state.isInString) {
            ZcodeCommandSlot__inStringParse(c);
        } else if (ZcodeCommandSlot__isWhitespace(c)) {
            // drops out.
        } else if (slot->state.isInBig) {
            ZcodeCommandSlot__inBigParse(c);
        } else if (slot->state.isInField) {
            ZcodeCommandSlot__inFieldParse(c);
        } else {
            ZcodeCommandSlot__outerCommandParse(c);
        }
    }
    return !wasWaiting; // waiting on run is the only circumstance where we don't want data.
}

void ZcodeCommandSlot_finish(ZcodeCommandSlot *slot) {
    slot->state.waitingOnRun = false;
    if (!slot->state.hasFailed) {
        if (slot->terminator == EOL_SYMBOL) {
            ZcodeCommandSlot__resetToSequence();
        } else if (slot->terminator == ANDTHEN_SYMBOL) {
            ZcodeCommandSlot__resetToCommand();
        } else if (slot->terminator == ORELSE_SYMBOL) {
            slot->state.sequenceDone = true;
        }
        slot->starter = slot->terminator;
    } else {
        if (slot->terminator == EOL_SYMBOL) {
            ZcodeCommandSlot__resetToSequence();
            slot->starter = slot->terminator;
        } else if (slot->terminator == ORELSE_SYMBOL) {
            if (slot->respStatus >= CMD_FAIL) {
                ZcodeCommandSlot__resetToCommand();
                slot->starter = slot->terminator;
            }
        }
    }
}
#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODECOMMANDSLOTCCODE_H_ */

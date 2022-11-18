/*
 * ZcodeCommandSlot.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODECOMMANDSLOT_H_
#define SRC_MAIN_C_ZCODE_ZCODECOMMANDSLOT_H_
#include "../ZcodeIncludes.h"
#include "ZcodeBigField.h"
#include "ZcodeFieldMap.h"

struct ZcodeCommandSlotParsingState {
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
};
typedef struct ZcodeCommandSlotParsingState ZcodeCommandSlotParsingState;

struct ZcodeCommandSlotStatus {
    bool isFirstCommand :1;
    bool isBroadcast :1;
    bool hasData :1;

    bool hasWrittenTerminator :1;
    bool hasOutLock :1;
    bool quietEnd :1;
};
typedef struct ZcodeCommandSlotStatus ZcodeCommandSlotStatus;

struct ZcodeCommandSlot {
    ZcodeFieldMap fieldMap;
    ZcodeBigField bigField;

    ZcodeCommandSlotParsingState state;
    uint16_t respStatus;
    ZcodeCommandSlotStatus runStatus;
    char terminator;
    char starter;
};
typedef struct ZcodeCommandSlot ZcodeCommandSlot;

void ZcodeInitialiseSlot(ZcodeCommandSlot *slot);

bool ZcodeSlotAcceptByte(ZcodeCommandSlot *slot, char c); // returns false <=> the data couldn't be used and needs to be resent later.

void ZcodeCommandSlot_finish(ZcodeCommandSlot *slot);

void ZcodeCommandFail(ZcodeCommandSlot *commandSlot, ZcodeResponseStatus failStatus);
void ZcodeCommandMildFail(ZcodeCommandSlot *commandSlot, ZcodeResponseStatus failStatus);

#endif /* SRC_MAIN_C_ZCODE_ZCODECOMMANDSLOT_H_ */

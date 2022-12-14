/*
 * Zcode.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODE_H_
#define SRC_MAIN_C_ZCODE_ZCODE_H_
#include "ZcodeIncludes.h"
#include "parsing/ZcodeCommandSlot.h"

struct ZcodeState {
    bool activated :1;
    uint8_t targetSlot :4;
};

typedef struct ZcodeState ZcodeState;

struct Zcode {
    ZcodeCommandSlot slot;
    ZcodeState state;
};

typedef struct Zcode Zcode;

void ZcodeRunNext();
void ZcodeInitialise();
bool ZcodeAcceptByte(char c);

#endif /* SRC_MAIN_C_ZCODE_ZCODE_H_ */

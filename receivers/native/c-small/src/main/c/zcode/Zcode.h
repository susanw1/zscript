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

struct Zcode {
    bool activated;
    ZcodeCommandSlot slot;
};

typedef struct Zcode Zcode;

void ZcodeRunNext(Zcode *zcode);
void ZcodeInitialise(Zcode *zcode);
bool ZcodeAcceptByte(Zcode *zcode, char c);

#endif /* SRC_MAIN_C_ZCODE_ZCODE_H_ */

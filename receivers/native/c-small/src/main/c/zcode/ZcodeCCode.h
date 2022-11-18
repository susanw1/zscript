/*
 * ZcodeCCode.h
 *
 *  Created on: 18 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODECCODE_H_
#define SRC_MAIN_C_ZCODE_ZCODECCODE_H_

#include "Zcode.h"
#include "ZcodeRunnerCCode.h"

void ZcodeInitialise(Zcode *zcode) {
    zcode->activated = false;
    ZcodeInitialiseSlot(&zcode->slot);
}
bool ZcodeAcceptByte(Zcode *zcode, char c) {
    return ZcodeSlotAcceptByte(&zcode->slot, c);
}
#endif /* SRC_MAIN_C_ZCODE_ZCODECCODE_H_ */

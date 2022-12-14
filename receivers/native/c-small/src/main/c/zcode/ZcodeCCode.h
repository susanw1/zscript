/*
 * ZcodeCCode.h
 *
 *  Created on: 18 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODECCODE_H_
#define SRC_MAIN_C_ZCODE_ZCODECCODE_H_
#include "ZcodeCCodeInclude.h"

#include "Zcode.h"
#include "ZcodeRunnerCCode.h"

void ZcodeInitialise() {
    zcode.state.activated = false;
    ZcodeInitialiseSlot();
}
bool ZcodeAcceptByte(char c) {
    return ZcodeSlotAcceptByte(c);
}
#endif /* SRC_MAIN_C_ZCODE_ZCODECCODE_H_ */

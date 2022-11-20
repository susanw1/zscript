/*
 * ZcodeBigFieldCCode.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELDCCODE_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELDCCODE_H_

#include "ZcodeBigField.h"

void ZcodeBigFieldReset(ZcodeBigField *big) {
    big->pos = 0;
    big->state.inNibble = 0;
}

bool ZcodeBigFieldAddNibble(ZcodeBigField *big, uint8_t nibble) {
    if (big->pos == ZCODE_BIG_FIELD_SIZE) {
        return false;
    }
    if (big->state.inNibble) {
        big->state.currentHex = (uint8_t) (big->state.currentHex << 4);
        big->state.currentHex |= nibble;
        big->big[big->pos++] = big->state.currentHex;
        big->state.currentHex = 0;
    } else {
        big->state.currentHex = nibble;
    }
    big->state.inNibble = !big->state.inNibble;
    return true;
}

bool ZcodeBigFieldAddByte(ZcodeBigField *big, uint8_t b) {
    if (big->pos == ZCODE_BIG_FIELD_SIZE || big->state.inNibble) {
        return false;
    }
    big->big[big->pos++] = b;
    return true;
}

void ZcodeBigFieldCopyToOutput(ZcodeBigField *big) {
    if (big->pos > 0) {
        ZcodeOutStream_WriteBigHexField(big->big, big->pos);
    }
}

#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELDCCODE_H_ */

/*
 * ZcodeBigFieldCCode.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELDCCODE_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELDCCODE_H_

#include "ZcodeBigField.h"

void ZcodeBigFieldReset() {
    ZcodeBigField *big = &zcode.slot.bigField;
    big->pos = 0;
    big->inNibble = 0;
}

bool ZcodeBigFieldAddNibble(uint8_t nibble) {
    ZcodeBigField *big = &zcode.slot.bigField;
    if (big->pos == ZCODE_BIG_FIELD_SIZE) {
        return false;
    }
    if (big->inNibble) {
        big->big[big->pos++] = (uint8_t) ((big->currentHex << 4) | nibble);
        big->currentHex = 0;
    } else {
        big->currentHex = nibble;
    }
    big->inNibble = !big->inNibble;
    return true;
}

bool ZcodeBigFieldAddByte(uint8_t b) {
    ZcodeBigField *big = &zcode.slot.bigField;
    if (big->pos == ZCODE_BIG_FIELD_SIZE || big->inNibble) {
        return false;
    }
    big->big[big->pos++] = b;
    return true;
}

void ZcodeBigFieldCopyToOutput() {
    ZcodeBigField *big = &zcode.slot.bigField;
    if (big->pos > 0) {
        ZcodeOutStream_WriteBigHexField(big->big, big->pos);
    }
}

#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELDCCODE_H_ */

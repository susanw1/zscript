/*
 * ZcodeBigField.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELD_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELD_H_

#include "../ZcodeIncludes.h"

struct ZcodeBigField {
    uint8_t big[ZCODE_BIG_FIELD_SIZE];
    ZcodeBigFieldAddress_t pos;
    uint8_t currentHex;
    bool inNibble;
};
typedef struct ZcodeBigField ZcodeBigField;

void ZcodeBigFieldReset();
bool ZcodeBigFieldAddNibble(uint8_t nibble);
bool ZcodeBigFieldAddByte(uint8_t b);

void ZcodeBigFieldCopyToOutput();

#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELD_H_ */

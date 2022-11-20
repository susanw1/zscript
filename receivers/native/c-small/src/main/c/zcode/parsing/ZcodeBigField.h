/*
 * ZcodeBigField.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELD_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELD_H_

#include "../ZcodeIncludes.h"

struct ZcodeBigFieldInfo {
    uint8_t currentHex :4;
    bool inNibble :1;
};

struct ZcodeBigField {
    uint8_t big[ZCODE_BIG_FIELD_SIZE];
    ZcodeBigFieldAddress_t pos;
    ZcodeBigFieldInfo state;
};
typedef struct ZcodeBigField ZcodeBigField;

void ZcodeBigFieldReset(ZcodeBigField *big);
bool ZcodeBigFieldAddNibble(ZcodeBigField *big, uint8_t nibble);
bool ZcodeBigFieldAddByte(ZcodeBigField *big, uint8_t b);

void ZcodeBigFieldCopyToOutput(ZcodeBigField *big);

#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODEBIGFIELD_H_ */

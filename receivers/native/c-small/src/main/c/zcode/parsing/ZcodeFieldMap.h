/*
 * ZcodeFieldMap.hpp
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAP_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAP_H_

#include "../ZcodeIncludes.h"

struct ZcodeFieldMap {
    uint16_t values[ZCODE_FIELD_COUNT];
    char fields[ZCODE_FIELD_COUNT];
    uint8_t nibbleCount;
    uint8_t fieldCount;
};

typedef struct ZcodeFieldMap ZcodeFieldMap;

bool ZcodeFieldMapAdd16(char f, uint16_t v);

bool ZcodeFieldMapAddBlank(char f);

bool ZcodeFieldMapAdd4(uint8_t nibble);

bool ZcodeFieldMapAdd8(uint8_t byte);

bool ZcodeFieldMapHas(char f);

uint16_t ZcodeFieldMapGetDef(char f, uint16_t def);

bool ZcodeFieldMapGetDest(char f, uint16_t *dest);

int32_t ZcodeFieldMapGet(char f);

void ZcodeFieldMapReset();

void ZcodeFieldMapCopyToOutput();

#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAP_H_ */

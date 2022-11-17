/*
 * ZcodeFieldMap.hpp
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAP_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAP_H_

#include "../ZcodeIncludes.hpp"

struct ZcodeFieldMap {
    uint16_t values[ZCODE_FIELD_COUNT];
    char fields[ZCODE_FIELD_COUNT];
    uint8_t fieldCount = 0;
    uint8_t nibbleCount = 0;

};

typedef struct ZcodeFieldMap ZcodeFieldMap;

bool ZcodeFieldMapAdd16(ZcodeFieldMap *map, char f, uint16_t v);

bool ZcodeFieldMapAddBlank(ZcodeFieldMap *map, char f);

bool ZcodeFieldMapAdd4(ZcodeFieldMap *map, uint8_t nibble);

bool ZcodeFieldMapAdd8(ZcodeFieldMap *map, uint8_t byte);

bool ZcodeFieldMapHas(ZcodeFieldMap *map, char f);

uint16_t ZcodeFieldMapGet(ZcodeFieldMap *map, char f, uint16_t def);

bool ZcodeFieldMapGet(ZcodeFieldMap *map, char f, uint16_t *dest);

int32_t ZcodeFieldMapGet(ZcodeFieldMap *map, char f);

void ZcodeFieldMapReset(ZcodeFieldMap *map);

void ZcodeFieldMapCopyToOutput(ZcodeFieldMap *map);

#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAP_H_ */

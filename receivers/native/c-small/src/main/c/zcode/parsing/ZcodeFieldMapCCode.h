/*
 * ZcodeFieldMapCCode.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAPCCODE_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAPCCODE_H_
#include "ZcodeFieldMap.h"

bool ZcodeFieldMapAdd16(char f, uint16_t v) {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    map->nibbleCount = 0;
    if (map->fieldCount == ZCODE_FIELD_COUNT) {
        return false;
    }
    for (uint8_t i = 0; i < map->fieldCount; ++i) {
        if (map->fields[i] == f) {
            return false;
        }
    }
    map->fields[map->fieldCount] = f;
    map->values[map->fieldCount++] = v;
    return true;
}

bool ZcodeFieldMapAddBlank(char f) {
    return ZcodeFieldMapAdd16(f, 0);
}

bool ZcodeFieldMapAdd4(uint8_t nibble) {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    if (map->nibbleCount >= 4) {
        return false;
    }
    map->values[map->fieldCount - 1] = (uint16_t) ((map->values[map->fieldCount - 1] << 4) | nibble);
    map->nibbleCount++;
    return true;
}

bool ZcodeFieldMapAdd8(uint8_t byte) {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    if (map->nibbleCount >= 3) {
        return false;
    }
    map->values[map->fieldCount - 1] = (uint16_t) ((map->values[map->fieldCount - 1] << 8) | byte);
    map->nibbleCount++;
    return true;
}

bool ZcodeFieldMapHas(char f) {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    for (uint8_t i = 0; i < map->fieldCount; i++) {
        if (map->fields[i] == f) {
            return true;
        }
    }
    return false;
}

uint16_t ZcodeFieldMapGetDef(char f, uint16_t def) {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    for (uint8_t i = 0; i < map->fieldCount; i++) {
        if (map->fields[i] == f) {
            return map->values[i];
        }
    }
    return def;
}

bool ZcodeFieldMapGetDest(char f, uint16_t *dest) {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    for (uint8_t i = 0; i < map->fieldCount; i++) {
        if (map->fields[i] == f) {
            *dest = map->values[i];
            return true;
        }
    }
    return false;
}

int32_t ZcodeFieldMapGet(char f) {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    for (uint8_t i = 0; i < map->fieldCount; i++) {
        if (map->fields[i] == f) {
            return map->values[i];
        }
    }
    return -1;
}

void ZcodeFieldMapReset() {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    map->fieldCount = 0;
    map->nibbleCount = 0;
}

void ZcodeFieldMapCopyToOutput() {
    ZcodeFieldMap *map = &zcode.slot.fieldMap;
    for (uint8_t i = 0; i < map->fieldCount; i++) {
        if (map->fields[i] != ECHO_PARAM && map->fields[i] != CMD_PARAM && map->fields[i] != STATUS_RESP_PARAM) {
            ZcodeOutStream_WriteField16(map->fields[i], map->values[i]);
        }
    }
}

#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAPCCODE_H_ */

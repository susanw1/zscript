/*
 * ZcodeFieldMapCCode.h
 *
 *  Created on: 17 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAPCCODE_H_
#define SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAPCCODE_H_
#include "ZcodeFieldMap.h"

bool ZcodeFieldMapAdd16(ZcodeFieldMap *map, char f, uint16_t v) {
    map->state.nibbleCount = 0;
    if (map->state.fieldCount == ZCODE_FIELD_COUNT) {
        return false;
    }
    for (uint8_t i = 0; i < map->state.fieldCount; ++i) {
        if (map->fields[i] == f) {
            return false;
        }
    }
    map->fields[map->state.fieldCount] = f;
    map->values[map->state.fieldCount++] = v;
    return true;
}

bool ZcodeFieldMapAddBlank(ZcodeFieldMap *map, char f) {
    return ZcodeFieldMapAdd16(map, f, 0);
}

bool ZcodeFieldMapAdd4(ZcodeFieldMap *map, uint8_t nibble) {
    if (map->state.nibbleCount >= 4) {
        return false;
    }
    map->values[map->state.fieldCount - 1] = (uint16_t) ((map->values[map->state.fieldCount - 1] << 4) | nibble);
    map->state.nibbleCount++;
    return true;
}

bool ZcodeFieldMapAdd8(ZcodeFieldMap *map, uint8_t byte) {
    if (map->state.nibbleCount >= 3) {
        return false;
    }
    map->values[map->state.fieldCount - 1] = (uint16_t) ((map->values[map->state.fieldCount - 1] << 8) | byte);
    map->state.nibbleCount++;
    return true;
}

bool ZcodeFieldMapHas(ZcodeFieldMap *map, char f) {
    for (uint8_t i = 0; i < map->state.fieldCount; i++) {
        if (map->fields[i] == f) {
            return true;
        }
    }
    return false;
}

uint16_t ZcodeFieldMapGetDef(ZcodeFieldMap *map, char f, uint16_t def) {
    for (uint8_t i = 0; i < map->state.fieldCount; i++) {
        if (map->fields[i] == f) {
            return map->values[i];
        }
    }
    return def;
}

bool ZcodeFieldMapGetDest(ZcodeFieldMap *map, char f, uint16_t *dest) {
    for (uint8_t i = 0; i < map->state.fieldCount; i++) {
        if (map->fields[i] == f) {
            *dest = map->values[i];
            return true;
        }
    }
    return false;
}

int32_t ZcodeFieldMapGet(ZcodeFieldMap *map, char f) {
    for (uint8_t i = 0; i < map->state.fieldCount; i++) {
        if (map->fields[i] == f) {
            return map->values[i];
        }
    }
    return -1;
}

void ZcodeFieldMapReset(ZcodeFieldMap *map) {
    map->state.fieldCount = 0;
    map->state.nibbleCount = 0;
}

void ZcodeFieldMapCopyToOutput(ZcodeFieldMap *map) {
    for (uint8_t i = 0; i < map->state.fieldCount; i++) {
        if (map->fields[i] != ECHO_PARAM && map->fields[i] != CMD_PARAM && map->fields[i] != STATUS_RESP_PARAM) {
            ZcodeOutStream_WriteField16(map->fields[i], map->values[i]);
        }
    }
}

#endif /* SRC_MAIN_C_ZCODE_PARSING_ZCODEFIELDMAPCCODE_H_ */

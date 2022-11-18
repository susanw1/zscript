/*
 * ZcodeAbstractOutStream.h
 *
 *  Created on: 18 Nov 2022
 *      Author: alicia
 */

#ifndef SRC_MAIN_C_ZCODE_ZCODEABSTRACTOUTSTREAM_H_
#define SRC_MAIN_C_ZCODE_ZCODEABSTRACTOUTSTREAM_H_

#include "ZcodeOutStream.h"
#include "ZcodeIncludes.h"

uint16_t ZcodeOutStream_recentStatus = OK;
bool ZcodeOutStream_LockValue = false;
uint8_t ZcodeOutStream_recentBreak = '\n';

uint8_t ZcodeOutStream_toHexDigit(uint8_t i) {
    return (uint8_t) (i > 9 ? 'a' + i - 10 : '0' + i);
}

bool ZcodeOutStream_Lock() {
    if (!ZcodeOutStream_LockValue) {
        ZcodeOutStream_LockValue = true;
        return true;
    }
    return false;
}

void ZcodeOutStream_Unlock() {
    ZcodeOutStream_LockValue = false;
}

bool ZcodeOutStream_IsLocked() {
    return ZcodeOutStream_LockValue;
}

void ZcodeOutStream_WriteBytes(uint8_t const *value, uint16_t l) {
    for (uint16_t i = 0; i < l; i++) {
        ZcodeOutStream_WriteByte(value[i]);
    }
}

void ZcodeOutStream_MarkDebug() {
    ZcodeOutStream_WriteByte(DEBUG_PREFIX);
}

void ZcodeOutStream_MarkNotification(ZcodeNotificationType type) {
    ZcodeOutStream_WriteByte(NOTIFY_PREFIX);
    ZcodeOutStream_WriteField16_((uint16_t) type);
}

void ZcodeOutStream_MarkBroadcast() {
    ZcodeOutStream_WriteByte(BROADCAST_PREFIX);
}

void ZcodeOutStream_MarkAddressing() {
    ZcodeOutStream_WriteByte(ADDRESS_PREFIX);
}

void ZcodeOutStream_MarkAddressingContinue() {
    ZcodeOutStream_WriteByte(ADDRESS_SEPARATOR);
}

void ZcodeOutStream_SilentSucceed() {
    ZcodeOutStream_recentStatus = OK;
}

void ZcodeOutStream_WriteStatus(uint16_t st) {
    ZcodeOutStream_recentStatus = (uint16_t) st;
    ZcodeOutStream_WriteByte(STATUS_RESP_PARAM);
    if (st != OK) {
        if (st >= 0x10) {
            if (st >= 0x100) {
                if (st >= 0x1000) {
                    ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit((uint8_t) (st >> 12) & 0x0F));
                }
                ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit((uint8_t) (st >> 8) & 0x0F));
            }
            ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit((uint8_t) (st >> 4) & 0x0F));
        }
        ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit(st & 0x0F));
    }
}

void ZcodeOutStream_WriteBigHexField(const uint8_t *value, ZcodeBigFieldAddress_t length) {
    ZcodeOutStream_WriteByte(BIGFIELD_PREFIX_MARKER);
    for (ZcodeBigFieldAddress_t i = 0; i < length; i++) {
        ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit((uint8_t) (value[i] >> 4)));
        ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit(value[i] & 0x0F));
    }
}

void ZcodeOutStream_WriteBigStringField(const char *nullTerminated) {
    ZcodeBigFieldAddress_t i;
    for (i = 0; nullTerminated[i] != '\0'; ++i) {
    }
    ZcodeOutStream_WriteBigStringField_((const uint8_t*) nullTerminated, i);
}

void ZcodeOutStream_WriteBigStringField_(const uint8_t *value, ZcodeBigFieldAddress_t length) {
    ZcodeOutStream_WriteByte(BIGFIELD_QUOTE_MARKER);
    for (ZcodeBigFieldAddress_t i = 0; i < length; ++i) {
        if (value[i] == EOL_SYMBOL || value[i] == STRING_ESCAPE_SYMBOL || value[i] == BIGFIELD_QUOTE_MARKER) {
            ZcodeOutStream_WriteByte(STRING_ESCAPE_SYMBOL);
            ZcodeOutStream_ContinueField8(value[i]);
        } else {
            ZcodeOutStream_WriteByte(value[i]);
        }
    }
    ZcodeOutStream_WriteByte(BIGFIELD_QUOTE_MARKER);
}

void ZcodeOutStream_WriteCommandSeparator() {
    ZcodeOutStream_WriteByte(ANDTHEN_SYMBOL);
    ZcodeOutStream_recentBreak = (uint8_t) ANDTHEN_SYMBOL;
}

void ZcodeOutStream_WriteSequenceErrorHandler() {
    ZcodeOutStream_WriteByte(ORELSE_SYMBOL);
    ZcodeOutStream_recentBreak = (uint8_t) ORELSE_SYMBOL;
}

void ZcodeOutStream_WriteCommandSequenceSeparator() {
    ZcodeOutStream_WriteByte(EOL_SYMBOL);
    ZcodeOutStream_recentBreak = (uint8_t) EOL_SYMBOL;
}

void ZcodeOutStream_WriteField8(char f, uint8_t v) {
    ZcodeOutStream_WriteByte((uint8_t) f);
    ZcodeOutStream_WriteField8_(v);
}
void ZcodeOutStream_WriteField8_(uint8_t v) {
    if (v != 0) {
        if (v >= 0x10) {
            ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit((uint8_t) (v >> 4)));
        }
        ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit(v & 0x0F));
    }
}
void ZcodeOutStream_ContinueField8(uint8_t v) {
    ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit((uint8_t) (v >> 4)));
    ZcodeOutStream_WriteByte(ZcodeOutStream_toHexDigit(v & 0x0F));
}

void ZcodeOutStream_WriteField16(char f, uint16_t value) {
    if (value <= 0xFF) {
        ZcodeOutStream_WriteField8(f, (uint8_t) value);
    } else {
        ZcodeOutStream_WriteField8(f, (uint8_t) (value >> 8));
        ZcodeOutStream_ContinueField8((uint8_t) (value & 0xFF));
    }
}

void ZcodeOutStream_WriteField16_(uint16_t value) {
    if (value <= 0xFF) {
        ZcodeOutStream_WriteField8_((uint8_t) value);
    } else {
        ZcodeOutStream_WriteField8_((uint8_t) (value >> 8));
        ZcodeOutStream_ContinueField8((uint8_t) (value & 0xFF));
    }
}

void ZcodeOutStream_ContinueField16(uint16_t value) {
    ZcodeOutStream_ContinueField8((uint8_t) (value >> 8));
    ZcodeOutStream_ContinueField8((uint8_t) (value & 0xFF));
}

void ZcodeOutStream_WriteField32(char f, uint32_t value) {
    if (value <= 0xFF) {
        ZcodeOutStream_WriteField8(f, (uint8_t) value);
    } else if (value <= 0xFFFF) {
        ZcodeOutStream_WriteField8(f, (uint8_t) (value >> 8));
        ZcodeOutStream_ContinueField8((uint8_t) (value & 0xFF));
    } else {
        ZcodeOutStream_WriteField8(f, (uint8_t) (value >> 24));
        ZcodeOutStream_ContinueField8((uint8_t) ((value >> 16) & 0xFF));
        ZcodeOutStream_ContinueField8((uint8_t) ((value >> 8) & 0xFF));
        ZcodeOutStream_ContinueField8((uint8_t) (value & 0xFF));
    }
}

void ZcodeOutStream_WriteField32_(uint32_t value) {
    if (value <= 0xFF) {
        ZcodeOutStream_WriteField8_((uint8_t) value);
    } else if (value <= 0xFFFF) {
        ZcodeOutStream_WriteField8_((uint8_t) (value >> 8));
        ZcodeOutStream_ContinueField8((uint8_t) (value & 0xFF));
    } else {
        ZcodeOutStream_WriteField8_((uint8_t) (value >> 24));
        ZcodeOutStream_ContinueField8((uint8_t) ((value >> 16) & 0xFF));
        ZcodeOutStream_ContinueField8((uint8_t) ((value >> 8) & 0xFF));
        ZcodeOutStream_ContinueField8((uint8_t) (value & 0xFF));
    }
}
void ZcodeOutStream_ContinueField32(uint32_t value) {
    ZcodeOutStream_ContinueField8((uint8_t) (value >> 24));
    ZcodeOutStream_ContinueField8((uint8_t) ((value >> 16) & 0xFF));
    ZcodeOutStream_ContinueField8((uint8_t) ((value >> 8) & 0xFF));
    ZcodeOutStream_ContinueField8((uint8_t) (value & 0xFF));
}

void ZcodeOutStream_WriteField64(char f, uint64_t value) {
    if (value <= 0xFF) {
        ZcodeOutStream_WriteField8(f, (uint8_t) value);
    } else if (value <= 0xFFFFFFFF) {
        ZcodeOutStream_WriteField32(f, (uint32_t) value);
    } else {
        ZcodeOutStream_WriteField32(f, (uint32_t) (value >> 32));
        ZcodeOutStream_ContinueField32((uint32_t) (value & 0xFFFFFFFF));
    }
}

void ZcodeOutStream_WriteField64_(uint64_t value) {
    if (value <= 0xFF) {
        ZcodeOutStream_WriteField8_((uint8_t) value);
    } else if (value <= 0xFFFFFFFF) {
        ZcodeOutStream_WriteField32_((uint32_t) value);
    } else {
        ZcodeOutStream_WriteField32_((uint32_t) (value >> 32));
        ZcodeOutStream_ContinueField32((uint32_t) (value & 0xFFFFFFFF));
    }
}

void ZcodeOutStream_ContinueField64(uint64_t value) {
    ZcodeOutStream_ContinueField32((uint32_t) (value >> 32));
    ZcodeOutStream_ContinueField32((uint32_t) (value & 0xFFFFFFFF));
}

void ZcodeOutStream_WriteFieldL(char f, const uint8_t *value, uint8_t length) {
    if (length == 0) {
        ZcodeOutStream_WriteField8(f, 0);
        return;
    } else {
        ZcodeOutStream_WriteField8(f, *value++);
        length--;
    }
    while (length != 0) {
        length--;
        ZcodeOutStream_ContinueField8(*value++);
    }
}

#endif /* SRC_MAIN_C_ZCODE_ZCODEABSTRACTOUTSTREAM_H_ */

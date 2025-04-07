// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENIZER_H
#define _ZS_TOKENIZER_H

#include <zscript/tokenbuffer/TokenRingBufferWriter.h>

static const uint8_t ZSTOK_ADDRESSING_FIELD_KEY = 0x80;

// Normal max length of a numeric field's value (eg 0-0xffff)
static const uint8_t DEFAULT_MAX_NUMERIC_BYTES = 2;

static const uint8_t ZSTOK_NORMAL_SEQUENCE_END              = 0xf0;
static const uint8_t ZSTOK_ERROR_BUFFER_OVERRUN             = 0xf1;
static const uint8_t ZSTOK_ERROR_CODE_ODD_HEXPAIR_LENGTH    = 0xf2;
static const uint8_t ZSTOK_ERROR_CODE_FIELD_TOO_LONG        = 0xf3;
static const uint8_t ZSTOK_ERROR_CODE_STRING_NOT_TERMINATED = 0xf4;
static const uint8_t ZSTOK_ERROR_CODE_STRING_ESCAPING       = 0xf5;
static const uint8_t ZSTOK_ERROR_CODE_ILLEGAL_TOKEN         = 0xf6;

static const uint8_t ZSTOK_CMD_END_ANDTHEN     = 0xe1;
static const uint8_t ZSTOK_CMD_END_ORELSE      = 0xe2;
static const uint8_t ZSTOK_CMD_END_OPEN_PAREN  = 0xe3;
static const uint8_t ZSTOK_CMD_END_CLOSE_PAREN = 0xe4;

typedef struct ZStok_Tokenizer {
    ZStok_TokenWriter writer;
    uint8_t           escapingCount : 2; // 2 bit counter, from 2 to 0
    bool skipToNL : 1;
    bool bufferOvr : 1;
    bool tokenizerError : 1;
    bool addressing : 1;
    bool expectKeyTypeIndicator : 1;
    bool hexPaired : 1;
    bool isText : 1;
    bool isQuotedString : 1;

} ZStok_Tokenizer;


static void zstok_initTokenizer(ZStok_Tokenizer *pTokenizer, ZStok_TokenWriter writer);

static bool zstok_checkCapacity(const ZStok_Tokenizer *pTokenizer);

static void zstok_dataLost(ZStok_Tokenizer *pTokenizer);

static void zstok_accept(ZStok_Tokenizer *pTokenizer, uint8_t b);

static bool zstok_offer(ZStok_Tokenizer *pTokenizer, uint8_t b);

#endif //_ZS_TOKENIZER_H

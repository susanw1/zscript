// Zscript - Command System for Microcontrollers
// Copyright (c) 2025 Zscript team (Susan Witts, Alicia Witts)
// SPDX-License-Identifier: MIT

#ifndef _ZS_TOKENIZER_DEFS_H
#define _ZS_TOKENIZER_DEFS_H

#include <stdint.h>
#include <stdbool.h>

#include <zscript/model/Zchars.h>

#include "Tokenizer.h"

// Private functions declarations not used outside this unit

static void zstok_resetFlags_priv(ZStok_Tokenizer *pTokenizer);

static void zstok_acceptText_priv(ZStok_Tokenizer *pTokenizer, uint8_t b);

static void zstok_startNewToken(ZStok_Tokenizer *pTokenizer, uint8_t b);

static void zstok_initTokenizer(ZStok_Tokenizer *pTokenizer, const ZStok_TokenWriter writer, uint8_t maxNumericBytes) {
    pTokenizer->writer          = writer;
    pTokenizer->maxNumericBytes = maxNumericBytes & 0xf;
    zstok_resetFlags_priv(pTokenizer);
}

static void zstok_resetFlags_priv(ZStok_Tokenizer *pTokenizer) {
    pTokenizer->escapingCount  = 0;
    pTokenizer->skipToNL       = false;
    pTokenizer->bufferOvr      = false;
    pTokenizer->tokenizerError = false;
    pTokenizer->numeric        = false;
    pTokenizer->addressing     = false;
    pTokenizer->isText         = false;
    pTokenizer->isNormalString = true;
}

static bool zstok_checkCapacity(ZStok_Tokenizer *pTokenizer) {
    return zstok_checkAvailableCapacity(pTokenizer->writer, 3);
}

static void zstok_dataLost(ZStok_Tokenizer *pTokenizer) {
    if (!pTokenizer->bufferOvr) {
        zstok_fail(pTokenizer->writer, ZSTOK_ERROR_BUFFER_OVERRUN);
        pTokenizer->bufferOvr = true;
    }
}

static bool zstok_offer(ZStok_Tokenizer *pTokenizer, uint8_t b) {
    if (zstok_checkCapacity(pTokenizer) || zstok_isReaderBlocked(&pTokenizer->writer.tokenBuffer->flags)) {
        zstok_accept(pTokenizer, b);
        return true;
    }
    return false;
}

static void zstok_accept(ZStok_Tokenizer *pTokenizer, uint8_t b) {
    if ((!pTokenizer->isText && ZS_Zchars_shouldIgnore(b)) || ZS_Zchars_alwaysIgnore(b)) {
        return;
    }

    if (pTokenizer->bufferOvr || pTokenizer->tokenizerError || pTokenizer->skipToNL) {
        if (b == ZS_Zchars_EOL_SYMBOL) {
            if (pTokenizer->skipToNL) {
                if (!zstok_checkCapacity(pTokenizer)) {
                    zstok_dataLost(pTokenizer);
                    return;
                }
                zstok_writeMarker(pTokenizer->writer, ZSTOK_NORMAL_SEQUENCE_END);
            }
            zstok_resetFlags_priv(pTokenizer);
        }
        return;
    }
    if (!zstok_checkCapacity(pTokenizer)) {
        zstok_dataLost(pTokenizer);
        return;
    }

    if (!zstok_isTokenComplete(pTokenizer->writer)) {
        if (pTokenizer->isText) {
            // "text" is broadly interpreted: we're pushing non-numeric bytes into a current token
            zstok_acceptText_priv(pTokenizer, b);
            return;
        }

        uint8_t hex = ZS_Zchars_parseHex(b);
        if (hex != ZS_Zchars_PARSE_NOT_HEX_0X10) {
            if (pTokenizer->numeric) {
                // Check field length
                int currentLength = zstok_getCurrentWriteTokenLength(pTokenizer->writer);
                if (currentLength == pTokenizer->maxNumericBytes && !zstok_isInNibble(pTokenizer->writer)) {
                    zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_FIELD_TOO_LONG);
                    pTokenizer->tokenizerError = true;
                    return;
                }
                // Skip leading zeros
                if (currentLength == 0 && hex == 0) {
                    return;
                }
            }
            zstok_continueTokenNibble(pTokenizer->writer, hex);
            return;
        }

        // Check big field odd length
        if (!pTokenizer->numeric && zstok_getCurrentWriteTokenKey(pTokenizer->writer) == ZS_Zchars_BIGFIELD_HEX && zstok_isInNibble(pTokenizer->writer)) {
            zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_ODD_BIGFIELD_LENGTH);
            pTokenizer->tokenizerError = true;
            if (b == ZS_Zchars_EOL_SYMBOL) {
                // interesting case: the error above could be caused by b==Z_NEWLINE, but we've written an error marker, so just reset and return
                zstok_resetFlags_priv(pTokenizer);
            }
            return;
        }
    }

    zstok_startNewToken(pTokenizer, b);
}

static void zstok_startNewToken(ZStok_Tokenizer *pTokenizer, uint8_t b) {
    if (b == ZS_Zchars_EOL_SYMBOL) {
        zstok_writeMarker(pTokenizer->writer, ZSTOK_NORMAL_SEQUENCE_END);
        zstok_resetFlags_priv(pTokenizer);
        return;
    }
//#ifdef ZSCRIPT_SUPPORT_ADDRESSING
    if (pTokenizer->addressing && b != ZS_Zchars_ADDRESSING_CONTINUE) {
        zstok_startToken(pTokenizer->writer, ZSTOK_ADDRESSING_FIELD_KEY, false);
        zstok_continueTokenByte(pTokenizer->writer, b);
        pTokenizer->addressing     = false;
        pTokenizer->isText         = true;
        pTokenizer->escapingCount  = 0;
        pTokenizer->isNormalString = false;
        return;
    }
//#endif

    if (ZS_Zchars_isSeparator(b)) {
        uint8_t marker = 0;
        switch (b) {
            case ZS_Zchars_ANDTHEN:
                marker = ZSTOK_CMD_END_ANDTHEN;
                break;
            case ZS_Zchars_ORELSE:
                marker = ZSTOK_CMD_END_ORELSE;
                break;
            case ZS_Zchars_OPENPAREN:
                marker = ZSTOK_CMD_END_OPEN_PAREN;
                break;
            case ZS_Zchars_CLOSEPAREN:
                marker = ZSTOK_CMD_END_CLOSE_PAREN;
                break;
                // more for other constructs?
        }
        if (marker != 0) {
            zstok_writeMarker(pTokenizer->writer, marker);
            return;
        }
        //TODO: die
    }

//#ifdef ZSCRIPT_SUPPORT_ADDRESSING
    if (b == ZS_Zchars_ADDRESSING) {
        pTokenizer->addressing = true;
    }
//#endif

    if (!ZS_Zchars_isAllowableKey(b)) {
        zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_ILLEGAL_TOKEN);
        pTokenizer->tokenizerError = true;
        return;
    }

    pTokenizer->numeric = !ZS_Zchars_isNonNumerical(b);
    pTokenizer->isText  = false;

    if (b != ZS_Zchars_COMMENT) {
        zstok_startToken(pTokenizer->writer, b, pTokenizer->numeric);
    } else {
        pTokenizer->skipToNL = true;
        return;
    }

    if (b == ZS_Zchars_BIGFIELD_QUOTED) {
        pTokenizer->isText         = true;
        pTokenizer->isNormalString = true;
        pTokenizer->escapingCount  = 0;
        return;
    }
}


static void zstok_acceptText_priv(ZStok_Tokenizer *pTokenizer, uint8_t b) {
    if (b == ZS_Zchars_EOL_SYMBOL) {
        if (pTokenizer->isNormalString) {
            zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_STRING_NOT_TERMINATED);
        } else {
            zstok_writeMarker(pTokenizer->writer, ZSTOK_NORMAL_SEQUENCE_END);
        }
        // we've written some marker, so reset as per the newline:
        zstok_resetFlags_priv(pTokenizer);
    } else if (pTokenizer->escapingCount > 0) {
        uint8_t hex = ZS_Zchars_parseHex(b);
        if (hex == ZS_Zchars_PARSE_NOT_HEX_0X10) {
            zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_STRING_ESCAPING);
            pTokenizer->tokenizerError = true;
        } else {
            zstok_continueTokenNibble(pTokenizer->writer, hex);
            pTokenizer->escapingCount--;
        }
    } else if (pTokenizer->isNormalString && b == ZS_Zchars_BIGFIELD_QUOTED) {
        zstok_endToken(pTokenizer->writer);
        pTokenizer->isText = false;
    } else if (pTokenizer->isNormalString && b == ZS_Zchars_STRING_ESCAPE) {
        pTokenizer->escapingCount = 2;
    } else {
        zstok_continueTokenByte(pTokenizer->writer, b);
    }
}


#endif //_ZS_TOKENIZER_DEFS_H

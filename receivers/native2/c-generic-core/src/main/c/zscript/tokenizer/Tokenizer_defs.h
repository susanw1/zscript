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
static void zstok_resetAllFlags_priv(ZStok_Tokenizer *pTokenizer);

inline static void zstok_resetTokenFlags_priv(ZStok_Tokenizer *pTokenizer);

static void zstok_acceptText_priv(ZStok_Tokenizer *pTokenizer, uint8_t b);

static void zstok_startNewToken_priv(ZStok_Tokenizer *pTokenizer, uint8_t b);

/**
 * Creates a new Tokenizer to write chars into tokens on the supplied writer.
 *
 * @param writer             the writer to write tokens to
 */
static void zstok_initTokenizer(ZStok_Tokenizer *pTokenizer, const ZStok_TokenWriter writer) {
    pTokenizer->writer = writer;
    zstok_resetAllFlags_priv(pTokenizer);
}

static void zstok_resetAllFlags_priv(ZStok_Tokenizer *pTokenizer) {
    pTokenizer->skipToNL       = false;
    pTokenizer->bufferOvr      = false;
    pTokenizer->tokenizerError = false;

    pTokenizer->addressing = false;
    zstok_resetTokenFlags_priv(pTokenizer);
}

/**
 * Resets just the flags that relate to the state of the current token, ready for the next one
 */
static void zstok_resetTokenFlags_priv(ZStok_Tokenizer *pTokenizer) {
    pTokenizer->expectKeyTypeIndicator = false;
    pTokenizer->hexPaired              = false;
    pTokenizer->isText                 = false;
    pTokenizer->isQuotedString         = true;
    pTokenizer->escapingCount          = 0;
}

/**
 * If a channel becomes aware that it has lost (or is about to lose) data, either because the channel has run out of buffer, or because the TokenBuffer is out of room, then it
 * can use this function to signal the Tokenizer to mark the current command sequence as overrun.
 */
static void zstok_dataLost(ZStok_Tokenizer *pTokenizer) {
    if (!pTokenizer->bufferOvr) {
        zstok_fail(pTokenizer->writer, ZSTOK_ERROR_BUFFER_OVERRUN);
        pTokenizer->bufferOvr = true;
    }
}

/**
 * Determine whether there is guaranteed capacity for another byte of input, in the worst case without knowing what that byte is.
 *
 * @return true if capacity definitely exists, false otherwise.
 */
static bool zstok_checkCapacity(const ZStok_Tokenizer *pTokenizer) {
    // A single char might a) write previous token's nibble, b) write the key+len of new token - hence 3.
    const int MOST_BYTES_REQUIRED_BY_ONE_CHAR = 3;
    return zstok_checkAvailableCapacity(pTokenizer->writer, MOST_BYTES_REQUIRED_BY_ONE_CHAR);
}

/**
 * Requests to process a byte of Zscript input into the tokenizer buffer, if there's capacity. If the offer returns true, then the byte has been consumed; otherwise the byte
 * was rejected, and it should be kept so that it can be presented again.
 *
 * @param b the new byte of zscript input
 * @return true if the byte was processed, false otherwise
 */
static bool zstok_offer(ZStok_Tokenizer *pTokenizer, uint8_t b) {
    if (zstok_checkCapacity(pTokenizer) || zstok_isReaderBlocked(&pTokenizer->writer.tokenBuffer->flags)) {
        zstok_accept(pTokenizer, b);
        return true;
    }
    return false;
}

/**
 * Process a byte of Zscript input into the tokenizer buffer.
 *
 * @param b the new byte of zscript input
 */
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
            zstok_resetAllFlags_priv(pTokenizer);
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

        if (pTokenizer->expectKeyTypeIndicator && b == ZS_Zchars_STRING_TYPE_QUOTED
            && !ZS_Zchars_isShortNumber(zstok_getCurrentWriteTokenKey(pTokenizer->writer))) {
            pTokenizer->isText         = true;
            pTokenizer->isQuotedString = true;
            pTokenizer->escapingCount  = 0;
            return;
        }
        pTokenizer->expectKeyTypeIndicator = false;

        const uint8_t hex = ZS_Zchars_parseHex(b);
        if (hex != ZS_Zchars_PARSE_NOT_HEX_0X10) {
            if (zstok_getCurrentWriteTokenLength(pTokenizer->writer) == DEFAULT_MAX_NUMERIC_BYTES) {
                if (!zstok_isInNibble(pTokenizer->writer) && ZS_Zchars_isShortNumber(zstok_getCurrentWriteTokenKey(pTokenizer->writer))) {
                    zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_FIELD_TOO_LONG);
                    pTokenizer->tokenizerError = true;
                    return;
                }
                pTokenizer->hexPaired = true;
            }
            zstok_continueTokenNibble(pTokenizer->writer, hex);
            return;
        }
    }

    zstok_startNewToken_priv(pTokenizer, b);
}

static void zstok_startNewToken_priv(ZStok_Tokenizer *pTokenizer, uint8_t b) {
    // Check long hex odd length - disallow non-numeric fields with incomplete nibbles
    if (pTokenizer->hexPaired && zstok_isInNibble(pTokenizer->writer)) {
        zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_ODD_HEXPAIR_LENGTH);
        pTokenizer->tokenizerError = true;
        if (b == ZS_Zchars_EOL_SYMBOL) {
            // interesting case: the error above could be caused by b==Z_NEWLINE, but we've written an error marker, so just reset and return
            zstok_resetAllFlags_priv(pTokenizer);
        }
        return;
    }

    if (b == ZS_Zchars_EOL_SYMBOL) {
        zstok_writeMarker(pTokenizer->writer, ZSTOK_NORMAL_SEQUENCE_END);
        zstok_resetAllFlags_priv(pTokenizer);
        return;
    }
//#ifdef ZSCRIPT_SUPPORT_ADDRESSING
    if (pTokenizer->addressing && b != ZS_Zchars_ADDRESSING_CONTINUE) {
        zstok_startToken(pTokenizer->writer, ZSTOK_ADDRESSING_FIELD_KEY);
        zstok_continueTokenByte(pTokenizer->writer, b);
        pTokenizer->addressing     = false;
        pTokenizer->isText         = true;
        pTokenizer->escapingCount  = 0;
        pTokenizer->isQuotedString = false;
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


    if (b != ZS_Zchars_COMMENT) {
        zstok_resetTokenFlags_priv(pTokenizer);
        zstok_startToken(pTokenizer->writer, b);
        pTokenizer->expectKeyTypeIndicator = true;
    } else {
        pTokenizer->skipToNL = true;
    }
}


/**
 * This is essentially copying bytes into the data of a token. Special cases include:
 * <ul>
 * <li>{@link Zchars#Z_EOL_SYMBOL} End-of-line - which terminates the string, or is a missing-quote error if it's a quoted string</li>
 * <li>{@link Zchars#Z_STRING_ESCAPE} String escapes in quoted strings</li>
 * <li>{@link Zchars#Z_STRING_TYPE_QUOTED} End-quote in quoted strings - which terminates the string</li>
 * </ul>
 * All of the above special characters must be escaped in quoted strings
 *
 * @param b the text byte to accept
 */
static void zstok_acceptText_priv(ZStok_Tokenizer *pTokenizer, uint8_t b) {
    if (b == ZS_Zchars_EOL_SYMBOL) {
        if (pTokenizer->isQuotedString) {
            zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_STRING_NOT_TERMINATED);
        } else {
            zstok_writeMarker(pTokenizer->writer, ZSTOK_NORMAL_SEQUENCE_END);
        }
        // we've written some marker, so reset as per the newline:
        zstok_resetAllFlags_priv(pTokenizer);
    } else if (pTokenizer->escapingCount > 0) {
        uint8_t hex = ZS_Zchars_parseHex(b);
        if (hex == ZS_Zchars_PARSE_NOT_HEX_0X10) {
            zstok_fail(pTokenizer->writer, ZSTOK_ERROR_CODE_STRING_ESCAPING);
            pTokenizer->tokenizerError = true;
        } else {
            zstok_continueTokenNibble(pTokenizer->writer, hex);
            pTokenizer->escapingCount--;
        }
    } else if (pTokenizer->isQuotedString && b == ZS_Zchars_STRING_TYPE_QUOTED) {
        zstok_endToken(pTokenizer->writer);
        pTokenizer->isText = false;
    } else if (pTokenizer->isQuotedString && b == ZS_Zchars_STRING_ESCAPE) {
        pTokenizer->escapingCount = 2;
    } else {
        zstok_continueTokenByte(pTokenizer->writer, b);
    }
}


#endif //_ZS_TOKENIZER_DEFS_H

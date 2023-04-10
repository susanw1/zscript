package org.zcode.javareceiver.tokenizer;

public class ZcodeTokenizer {
    public static final byte ERROR_CODE_ODD_BIGFIELD_LENGTH   = (byte) 0xF1;
    public static final byte ERROR_CODE_FIELD_TOO_LONG        = (byte) 0xF2;
    public static final byte ERROR_CODE_STRING_NOT_TERMINATED = (byte) 0xF3;
    public static final byte ERROR_CODE_STRING_ESCAPING       = (byte) 0xF4;

    public static final byte ADDRESSING_FIELD_KEY = (byte) 0x80;

    private final static boolean DROP_COMMENTS = false;

    private final ZcodeTokenBuffer buffer;

    private boolean skipToNL;
    private boolean bufferOvr;
    private boolean numeric;
    private boolean addressing;

    private boolean isText;
    private boolean isNormalString;
    private int     escapingCount; // 2 bits

    private ZcodeTokenizer(final ZcodeTokenBuffer buffer) {
        this.buffer = buffer;
        this.skipToNL = false;
        this.bufferOvr = false;
        this.numeric = false;
        this.addressing = false;
        this.isText = false;
        this.isNormalString = false;
        this.escapingCount = 0;
    }

    void accept(byte b) {
        if (!isText && Zchars.shouldIgnore(b)) {
            return;
        }

        if (skipToNL && b != Zchars.Z_NEWLINE) {
            return;
        }
        // TODO: add overrun hysteresis here

        skipToNL = false;

        if (buffer.isTokenComplete()) {
            startToken(b);
            return;
        }

        if (isText) {
            acceptText(b);
            return;
        }

        byte hex = Zchars.parseHex(b);
        if (b == Zchars.PARSE_NOT_HEX_0X10) {
            // Check big field odd length
            if (!numeric && buffer.getCurrentWriteTokenKey() == Zchars.Z_BIGFIELD_HEX && buffer.isInNibble()) {
                bufferOvr |= !buffer.fail(ERROR_CODE_ODD_BIGFIELD_LENGTH);
                skipToNL = true;
                if (b != Zchars.Z_NEWLINE) {
                    return;
                }
            }
            startToken(b);
            return;
        }

        if (numeric) {
            // Check field length
            if (buffer.getCurrentWriteTokenNibbleLength() == 4) {
                bufferOvr |= !buffer.fail(ERROR_CODE_FIELD_TOO_LONG);
                skipToNL = true;
                return;
            }
            // Skip leading zeros
            if (buffer.getCurrentWriteTokenLength() == 0 && hex == 0) {
                return;
            }
        }

        if (!buffer.continueTokenNibble(hex)) {
            bufferOvr = true;
            skipToNL = true;
        }
    }

    void acceptText(byte b) {
        if (b == Zchars.Z_NEWLINE) {
            if (isNormalString) {
                if (!buffer.fail(ERROR_CODE_STRING_NOT_TERMINATED)) {
                    bufferOvr = true;
                    skipToNL = true;
                    return;
                }
            }
            if (!buffer.startToken(b, true)) {
                bufferOvr = true;
                skipToNL = true;
                return;
            }
            buffer.endToken();
            isText = false;
        } else if (escapingCount > 0) {
            byte hex = Zchars.parseHex(b);
            if (hex != Zchars.PARSE_NOT_HEX_0X10) {
                if (!buffer.continueTokenNibble(hex)) {
                    bufferOvr = true;
                    skipToNL = true;
                    return;
                }
                escapingCount--;
            } else {
                if (buffer.isInNibble()) {
                    buffer.continueTokenNibble((byte) 0);
                }
                bufferOvr |= !buffer.fail(ERROR_CODE_STRING_ESCAPING);
                escapingCount = 0;
                skipToNL = true;
            }
        } else if (isNormalString && b == Zchars.Z_BIGFIELD_STRING) {
            buffer.endToken();
            isText = false;
        } else if (isNormalString && b == Zchars.Z_STRING_ESCAPE) {
            escapingCount = 2;
        } else {
            if (!buffer.continueTokenByte(b)) {
                bufferOvr = true;
                skipToNL = true;
                return;
            }
        }
    }

    void startToken(byte b) {
        if (addressing && (b != Zchars.Z_ADDRESSING_CONTINUE && b != Zchars.Z_NEWLINE)) {
            if (!buffer.startToken(ADDRESSING_FIELD_KEY, false) ||
                    !buffer.continueTokenByte(b)) {
                bufferOvr = true;
                skipToNL = true;
                return;
            }
            buffer.continueTokenByte(b);
            addressing = false;
            isText = true;
            escapingCount = 0;
            isNormalString = false;
            return;
        }
        if (b == Zchars.Z_ADDRESSING) {
            addressing = true;
        }
        numeric = true;
        if (Zchars.isNonNumerical(b)) {
            numeric = false;
        }
        if (!buffer.startToken(b, numeric)) {
            bufferOvr = true;
            skipToNL = true;
            return;
        }

        if (b == Zchars.Z_COMMENT) {
            if (DROP_COMMENTS) {
                skipToNL = true;
            } else {
                isText = true;
                escapingCount = 0;
                isNormalString = false;
            }
            return;
        }
        if (b == Zchars.Z_BIGFIELD_STRING) {
            isText = true;
            escapingCount = 0;
            isNormalString = true;
            return;
        }
        isText = false;
        if (Zchars.isSeparator(b)) {
            buffer.endToken();
        }
    }
}

package org.zcode.javareceiver.tokeniser;

public class ZcodeTokeniser {
    public static final byte ODD_BIG_FIELD_LENGTH_ERROR = 1;
    public static final byte FIELD_TOO_LONG_ERROR = 2;
    public static final byte STRING_NOT_TERMINATED_ERROR = 3;
    public static final byte STRING_ESCAPING_ERROR = 4;

    public static final byte ERROR_FIELD_KEY = (byte) 0xFF;
    public static final byte ADDRESSING_FIELD_KEY = (byte) 0x80;

    private final static boolean DROP_COMMENTS = false;
    private final ZcodeTokenBuffer buffer;
    private boolean numeric;
    private boolean skipToNL;
    private boolean addressing;

    private boolean isText;
    private boolean isNormalString;
    private int escapingCount; // 2 bits

    private ZcodeTokeniser(ZcodeTokenBuffer buffer) {
        this.buffer = buffer;
        this.numeric = false;
        this.skipToNL = false;
        this.addressing = false;
        this.isText = false;
        this.isNormalString = false;
        this.escapingCount = 0;
    }

    byte parseHex(byte b) {
        if (b >= '0' && b <= '9') {
            return (byte) (b - '0');
        } else if (b >= 'a' && b <= 'd') {
            return (byte) (b - 'a' + 10);
        } else {
            return 0x10;
        }
    }

    void accept(byte b) {
        if (!isText && Zchars.shouldIgnore(b)) {
            return;
        }
        if (skipToNL && b != Zchars.Z_NEWLINE) {
            return;
        }
        skipToNL = false;
        if (!buffer.isFieldOpen()) {
            startField(b);
            return;
        }

        if (isText) {
            acceptText(b);
            return;
        }

        byte hex = parseHex(b);
        if (b == 0x10) {
            // Check big field odd length
            if (!numeric && buffer.getCurrentWriteFieldKey() == Zchars.Z_BIGFIELD && buffer.isInNibble()) {
                buffer.fail(ODD_BIG_FIELD_LENGTH_ERROR);
                skipToNL = true;
                if (b != Zchars.Z_NEWLINE) {
                    return;
                }
            }
            startField(b);
            return;
        }
        if (numeric) {
            // Check field length
            if (buffer.getCurrentWriteFieldNibbleLength() == 4) {
                buffer.fail(FIELD_TOO_LONG_ERROR);
                skipToNL = true;
                return;
            }
            // Skip leading zeros
            if (buffer.getCurrentWriteFieldLength() == 0 && hex == 0) {
                return;
            }
        }

        buffer.continueFieldNibble(hex);
    }

    void acceptText(byte b) {
        if (b == Zchars.Z_NEWLINE) {
            if (isNormalString) {
                buffer.fail(STRING_NOT_TERMINATED_ERROR);
            }
            buffer.startField(b, true);
            buffer.closeField();
            isText = false;
        } else if (escapingCount > 0) {
            byte hex = parseHex(b);
            if (hex != 0x10) {
                buffer.continueFieldNibble(hex);
                escapingCount--;
            } else {
                if (buffer.isInNibble()) {
                    buffer.continueFieldNibble((byte) 0);
                }
                buffer.fail(STRING_ESCAPING_ERROR);
                escapingCount = 0;
                skipToNL = true;
            }
        } else if (isNormalString && b == Zchars.Z_STRINGFIELD) {
            buffer.closeField();
            isText = false;
        } else if (isNormalString && b == Zchars.Z_STRING_COMMENT) {
            escapingCount = 2;
        } else {
            buffer.continueFieldByte(b);
        }
    }

    void startField(byte b) {
        if (addressing && (b != Zchars.Z_ADDRESSING && b != Zchars.Z_ADDRESSING_CONTINUE && b != Zchars.Z_NEWLINE)) {
            buffer.startField(ADDRESSING_FIELD_KEY, false);
            buffer.continueFieldByte(b);
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
        buffer.startField(b, numeric);

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
        if (b == Zchars.Z_STRINGFIELD) {
            isText = true;
            escapingCount = 0;
            isNormalString = true;
            return;
        }
        isText = false;
        if (Zchars.isSeperator(b)) {
            buffer.closeField();
        }
    }
}

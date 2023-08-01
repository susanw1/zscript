package net.zscript.javareceiver.tokenizer;

public class Zchars {
    public static final byte Z_CMD                 = 'Z';
    public static final byte Z_NEWLINE             = '\n';
    public static final byte Z_ANDTHEN             = '&';
    public static final byte Z_ORELSE              = '|';
    public static final byte Z_OPENPAREN           = '(';
    public static final byte Z_CLOSEPAREN          = ')';
    public static final byte Z_LOCKS               = '%';
    public static final byte Z_BIGFIELD_HEX        = '+';
    public static final byte Z_BIGFIELD_QUOTED     = '"';
    public static final byte Z_STRING_ESCAPE       = '=';
    public static final byte Z_COMMENT             = '#';
    public static final byte Z_ADDRESSING          = '@';
    public static final byte Z_ADDRESSING_CONTINUE = '.';
    public static final byte Z_ECHO                = '_';
    public static final byte Z_STATUS              = 'S';

    public static boolean alwaysIgnore(byte b) {
        return b == '\0';
    }

    public static boolean shouldIgnore(byte b) {
        return b == ' ' || b == '\r' || b == '\t' || b == ',' || b == '\0';
    }

    public static boolean isSeparator(byte b) {
        return b == Z_NEWLINE || b == Z_ANDTHEN || b == Z_ORELSE || b == Z_OPENPAREN || b == Z_CLOSEPAREN;
    }

    public static boolean isNonNumerical(byte b) {
        return b == Z_LOCKS || b == Z_BIGFIELD_HEX || b == Z_COMMENT || b == Z_BIGFIELD_QUOTED;
    }

    public static boolean isBigField(byte b) {
        return b == Z_BIGFIELD_HEX || b == Z_BIGFIELD_QUOTED;
    }

    public static boolean isNumericKey(byte b) {
        return (b >= 'A' && b <= 'Z');
    }

    public static boolean isCommandKey(byte b) {
        return isNumericKey(b) || isBigField(b);
    }

    public static boolean isAllowableKey(byte b) {
        // disallow hex, non-printing and top-bit-set keys
        return b >= 0x20 && parseHex(b) == PARSE_NOT_HEX_0X10;
    }

    static final int PARSE_NOT_HEX_0X10 = 0x10;

    public static byte parseHex(byte b) {
        if (b >= '0' && b <= '9') {
            return (byte) (b - '0');
        }
        if (b >= 'a' && b <= 'f') {
            return (byte) (b - 'a' + 10);
        }
        return PARSE_NOT_HEX_0X10;
    }
}

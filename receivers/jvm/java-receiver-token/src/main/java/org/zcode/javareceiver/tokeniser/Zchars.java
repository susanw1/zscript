package org.zcode.javareceiver.tokeniser;

public class Zchars {
    public static final byte Z_NEWLINE             = '\n';
    public static final byte Z_AND                 = '&';
    public static final byte Z_OR                  = '|';
    public static final byte Z_LOCKS               = '%';
    public static final byte Z_BIGFIELD            = '+';
    public static final byte Z_STRINGFIELD         = '"';
    public static final byte Z_STRING_COMMENT      = '=';
    public static final byte Z_COMMENT             = '#';
    public static final byte Z_ADDRESSING          = '@';
    public static final byte Z_ADDRESSING_CONTINUE = '.';

    public static boolean shouldIgnore(byte b) {
        return b == ' ' || b == '\r' || b == '\t' || b == ',';
    }

    public static boolean isSeperator(byte b) {
        return b == Z_NEWLINE || b == Z_AND || b == Z_OR;
    }

    public static boolean isNonNumerical(byte b) {
        return b == Z_LOCKS || b == Z_BIGFIELD || b == Z_COMMENT || b == Z_STRINGFIELD;
    }
}

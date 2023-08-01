package net.zscript.javaclient.parsing;

public enum Zchars {
    CMD_PARAM('Z'),
    ECHO_PARAM('E'),
    STATUS_RESP_PARAM('S'),
    NOTIFY_TYPE_PARAM('Z'),

    ANDTHEN_SYMBOL('&'),
    ORELSE_SYMBOL('|'),
    EOL_SYMBOL('\n'),
    STRING_ESCAPE_SYMBOL('\\'),

    BIGFIELD_PREFIX_MARKER('+'),
    BIGFIELD_QUOTE_MARKER('"'),

    BROADCAST_PREFIX('*'),
    DEBUG_PREFIX('#'),
    PARALLEL_PREFIX('%'),
    NOTIFY_PREFIX('!'),
    ADDRESS_PREFIX('@');

    public final byte ch;

    Zchars(char ch) {
        this.ch = (byte) ch;
    }

    boolean matches(byte c) {
        return c == ch;
    }

    static boolean isCommandEnd(byte c) {
        return c == ANDTHEN_SYMBOL.ch || c == ORELSE_SYMBOL.ch || c == EOL_SYMBOL.ch;
    }

    static boolean isWhitespace(byte c) {
        return c == ' ' || c == '\r' || c == '\t' || c == ',';
    }

    static boolean isParamKey(byte c) {
        return c >= 'A' && c <= 'Z';
    }

    static byte getHex(byte c) {
        if (c >= 'a') {
            return (byte) (c <= 'f' ? c - 'a' + 10 : -1);
        } else {
            return (byte) (c >= '0' && c <= '9' ? c - '0' : -1);
        }
    }
}

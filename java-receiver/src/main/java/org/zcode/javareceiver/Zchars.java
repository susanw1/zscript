package org.zcode.javareceiver;

public enum Zchars {
    CMD_PARAM('Z'),
    ECHO_PARAM('E'),
    STATUS_RESP_PARAM('S'),
    NOTIFY_TYPE_PARAM('Z'),

    ANDTHEN_SYMBOL('&'),
    ORELSE_SYMBOL('|'),
    EOL_SYMBOL('\n'),

    BIGFIELD_PREFIX_MARKER('+'),
    BIGFIELD_QUOTE_MARKER('"'),
    
    BROADCAST_PREFIX('*'),
    DEBUG_PREFIX('#'),
    PARALLEL_PREFIX('%'),
    NOTIFY_PREFIX('!'),
    ADDRESS_PREFIX('@');

    public final char ch;

    Zchars(char ch) {
        this.ch = ch;
    }
}

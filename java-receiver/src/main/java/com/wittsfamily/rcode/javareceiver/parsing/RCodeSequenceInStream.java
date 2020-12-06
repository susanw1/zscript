package com.wittsfamily.rcode.javareceiver.parsing;

public interface RCodeSequenceInStream {
    char nextChar();

    boolean hasNextChar();

    RCodeLookaheadStream getLookahead();

    void openCommandSequence();

    void closeCommandSequence();

    boolean isCommandSequenceOpen();
}

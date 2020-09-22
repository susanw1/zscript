package com.wittsfamily.rcode.javareceiver.parsing;

public interface RCodeSequenceInStream {
    char nextChar();

    boolean hasNextChar();

    RCodeLookaheadStream getLookahead();

    boolean hasNextCommandSequence();

    void openCommandSequence();

    void closeCommandSequence();

    boolean isCommandSequenceOpen();
}

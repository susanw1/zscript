package com.wittsfamily.rcode.javareceiver.parsing;

@Deprecated
public interface OldRCodeSequenceInStream {
    char nextChar();

    boolean hasNextChar();

    RCodeLookaheadStream getLookahead();

    void openCommandSequence();

    void closeCommandSequence();

    boolean isCommandSequenceOpen();
}

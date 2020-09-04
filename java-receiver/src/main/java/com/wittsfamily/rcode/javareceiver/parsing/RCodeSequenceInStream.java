package com.wittsfamily.rcode.javareceiver.parsing;

public interface RCodeSequenceInStream {
    char next();

    boolean hasNext();

    RCodeLookaheadStream getLookahead();

    boolean hasNextCommandSequence();

    void openCommandSequence();

    void closeCommandSequence();

    boolean isCommandSequenceOpen();
}

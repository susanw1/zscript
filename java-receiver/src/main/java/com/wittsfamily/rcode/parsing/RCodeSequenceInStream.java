package com.wittsfamily.rcode.parsing;

public interface RCodeSequenceInStream {
    char next();

    boolean hasNext();

    RCodeLookaheadStream getLookahead();

    boolean hasNextCommandSequence();

    void openCommandSequence();

    void closeCommandSequence();

    boolean isCommandSequenceOpen();
}

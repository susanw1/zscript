package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeLookaheadStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeSequenceInStream;

public class RCodeVoidInStream implements RCodeSequenceInStream {

    @Override
    public char next() {
        return 0;
    }

    @Override
    public boolean hasNext() {
        return false;
    }

    @Override
    public RCodeLookaheadStream getLookahead() {
        return null;
    }

    @Override
    public boolean hasNextCommandSequence() {
        return false;
    }

    @Override
    public void openCommandSequence() {

    }

    @Override
    public void closeCommandSequence() {

    }

    @Override
    public boolean isCommandSequenceOpen() {
        return false;
    }

}

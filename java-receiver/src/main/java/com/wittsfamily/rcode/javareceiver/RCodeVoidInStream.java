package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeSequenceInStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeLookaheadStream;

public class RCodeVoidInStream implements RCodeChannelInStream {
    private final RCodeSequenceInStream seqInStream = new RCodeSequenceInStream(this);

    @Override
    public int read() {
        return -1;
    }

    @Override
    public RCodeSequenceInStream getSequenceInStream() {
        return seqInStream;
    }

    @Override
    public RCodeLookaheadStream getLookahead() {
        return new RCodeLookaheadStream() {

            @Override
            public char read() {
                return '\n';
            }
        };
    }
}

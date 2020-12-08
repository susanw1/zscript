package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeLookaheadStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeSequenceInStream;

public class StringInStream implements RCodeChannelInStream {
    private final RCodeSequenceInStream seqInStream = new RCodeSequenceInStream(this);
    private final String s;
    private int pos = 0;

    public StringInStream(String s) {
        this.s = s;
    }

    @Override
    public RCodeLookaheadStream getLookahead() {
        return new RCodeLookaheadStream() {
            int offset = 0;

            @Override
            public char read() {
                if (pos + offset >= s.length()) {
                    return '\n';
                }
                return s.charAt(pos + offset++);
            }
        };
    }

    @Override
    public RCodeSequenceInStream getSequenceInStream() {
        return seqInStream;
    }

    @Override
    public int read() {
        if (pos >= s.length()) {
            return -1;
        }
        return s.charAt(pos++);
    }

    public boolean hasNext() {
        return pos < s.length();
    }
}

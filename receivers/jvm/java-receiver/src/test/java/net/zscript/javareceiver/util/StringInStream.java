package net.zscript.javareceiver.util;

import net.zscript.javareceiver.instreams.ZcodeChannelInStream;
import net.zscript.javareceiver.instreams.ZcodeLookaheadStream;
import net.zscript.javareceiver.instreams.ZcodeSequenceInStream;

public class StringInStream implements ZcodeChannelInStream {
    private final ZcodeSequenceInStream seqInStream = new ZcodeSequenceInStream(this);

    private final String s;
    private int          pos = 0;

    public StringInStream(final String s) {
        this.s = s;
    }

    @Override
    public ZcodeLookaheadStream getLookahead() {
        return new ZcodeLookaheadStream() {
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
    public ZcodeSequenceInStream getSequenceInStream() {
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

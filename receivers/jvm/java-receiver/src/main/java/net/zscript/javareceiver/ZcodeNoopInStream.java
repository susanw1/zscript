package net.zscript.javareceiver;

import net.zscript.javareceiver.instreams.ZcodeChannelInStream;
import net.zscript.javareceiver.instreams.ZcodeLookaheadStream;
import net.zscript.javareceiver.instreams.ZcodeSequenceInStream;

public class ZcodeNoopInStream implements ZcodeChannelInStream {
    private final ZcodeSequenceInStream seqInStream = new ZcodeSequenceInStream(this);

    @Override
    public int read() {
        return -1;
    }

    @Override
    public ZcodeSequenceInStream getSequenceInStream() {
        return seqInStream;
    }

    @Override
    public ZcodeLookaheadStream getLookahead() {
        return new ZcodeLookaheadStream() {

            @Override
            public char read() {
                return Zchars.EOL_SYMBOL.ch;
            }
        };
    }
}

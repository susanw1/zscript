package org.zcode.javareceiver.executionspace;

import org.zcode.javareceiver.instreams.ZcodeChannelInStream;
import org.zcode.javareceiver.instreams.ZcodeLookaheadStream;
import org.zcode.javareceiver.instreams.ZcodeSequenceInStream;

public class ZcodeExecutionSpaceInStream implements ZcodeChannelInStream {
    private final ZcodeExecutionSpace   space;
    private final ZcodeSequenceInStream seqInStream = new ZcodeSequenceInStream(this);
    private int                         pos         = 0;
    private boolean                     inUse       = false;

    public ZcodeExecutionSpaceInStream(final ZcodeExecutionSpace space) {
        this.space = space;
    }

    public void setup(final int position) {
        pos = position;
        inUse = true;
    }

    public void release() {
        inUse = false;
    }

    public boolean isInUse() {
        return inUse;
    }

    @Override
    public int read() {
        if (pos >= space.getLength() || space.get(pos) == '\n') {
            return -1;
        } else {
            return (char) space.get(pos++);
        }
    }

    @Override
    public ZcodeLookaheadStream getLookahead() {
        return new ZcodeLookaheadStream() {
            private int offset = 0;

            @Override
            public char read() {
                if (pos + offset >= space.getLength() || space.get(pos + offset) == '\n') {
                    return '\n';
                } else {
                    return (char) space.get(pos + offset++);
                }
            }
        };
    }

    public int getPosition() {
        return pos;
    }

    public boolean needsDelayNext() {
        return pos >= space.getLength() || (pos == space.getLength() - 1 && space.get(pos) == '\n');
    }

    @Override
    public ZcodeSequenceInStream getSequenceInStream() {
        return seqInStream;
    }
}

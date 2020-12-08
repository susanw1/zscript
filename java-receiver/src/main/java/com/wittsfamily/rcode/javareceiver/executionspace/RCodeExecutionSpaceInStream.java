package com.wittsfamily.rcode.javareceiver.executionspace;

import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeLookaheadStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeSequenceInStream;

public class RCodeExecutionSpaceInStream implements RCodeChannelInStream {
    private final RCodeExecutionSpace space;
    private final RCodeSequenceInStream seqInStream = new RCodeSequenceInStream(this);
    private int pos = 0;
    private boolean inUse = false;

    public RCodeExecutionSpaceInStream(RCodeExecutionSpace space) {
        this.space = space;
    }

    public void setup(int position) {
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
    public RCodeLookaheadStream getLookahead() {
        return new RCodeLookaheadStream() {
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
    public RCodeSequenceInStream getSequenceInStream() {
        return seqInStream;
    }
}

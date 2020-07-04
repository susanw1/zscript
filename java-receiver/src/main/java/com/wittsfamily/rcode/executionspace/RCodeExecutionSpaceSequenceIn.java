package com.wittsfamily.rcode.executionspace;

import com.wittsfamily.rcode.parsing.RCodeLookaheadStream;
import com.wittsfamily.rcode.parsing.RCodeSequenceInStream;

public class RCodeExecutionSpaceSequenceIn implements RCodeSequenceInStream {
    private final RCodeExecutionSpace space;
    private int pos = 0;
    private boolean isSequenceOpen = false;
    private boolean hasStarted = false;
    private boolean inUse = false;

    public RCodeExecutionSpaceSequenceIn(RCodeExecutionSpace space) {
        this.space = space;
    }

    public void setup(int position) {
        pos = position;
        isSequenceOpen = false;
        hasStarted = false;
        inUse = true;
    }

    public void release() {
        inUse = false;
    }

    public boolean isInUse() {
        return inUse;
    }

    @Override
    public char next() {
        if (isSequenceOpen) {
            if (pos >= space.getLength() || space.get(pos) == '\n') {
                isSequenceOpen = false;
                return '\n';
            } else {
                return (char) space.get(pos++);
            }
        } else {
            return '\n';
        }
    }

    @Override
    public boolean hasNext() {
        return isSequenceOpen || !hasStarted;
    }

    @Override
    public RCodeLookaheadStream getLookahead() {
        return new RCodeLookaheadStream() {
            private boolean stillInSequence = isSequenceOpen;
            private int offset = 0;

            @Override
            public char read() {
                if (stillInSequence) {
                    if (pos - 1 + offset >= space.getLength() || space.get(pos - 1 + offset) == '\n') {
                        stillInSequence = false;
                        return '\n';
                    } else {
                        return (char) space.get(pos - 1 + offset++);
                    }
                } else {
                    return '\n';
                }
            }
        };
    }

    @Override
    public boolean hasNextCommandSequence() {
        return false;
    }

    @Override
    public void openCommandSequence() {
        isSequenceOpen = true;
        hasStarted = false;
        if (pos < space.getLength() && space.get(pos) == '\n') {
            pos++;
        }
        if (pos >= space.getLength()) {
            pos = 0;
        }
    }

    public int getPosition() {
        return pos;
    }

    public boolean needsDelayNext() {
        return pos >= space.getLength() || (pos == space.getLength() - 1 && space.get(pos) == '\n');
    }

    @Override
    public void closeCommandSequence() {
        if (isSequenceOpen) {
            isSequenceOpen = false;
            while (pos < space.getLength() && space.get(pos++) != '\n') {
            }
        }
    }

    @Override
    public boolean isCommandSequenceOpen() {
        return isSequenceOpen;
    }

}

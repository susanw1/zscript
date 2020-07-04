package com.wittsfamily.rcode.parsing;

public class RCodeInStream {
    private final RCodeSequenceInStream sequenceIn;
    private boolean hasReachedCommandEnd = false;
    private char current = 0;
    private boolean isInString = false;
    private boolean hasBackslash = false;
    private boolean lock = false;

    public RCodeInStream(RCodeSequenceInStream sequenceIn) {
        this.sequenceIn = sequenceIn;
    }

    public char read() {
        char c = readInt();
        if (c == 0) {
            return readInt();
        }
        return c;
    }

    private char readInt() {
        if (hasNext()) {
            char prev = current;
            current = sequenceIn.next();
            if (current == '\n' || (current == ';' && !isInString)) {
                hasReachedCommandEnd = true;
            } else if (current == '"') {
                if (!hasBackslash) {
                    isInString = !isInString;
                    hasBackslash = false;
                }
            } else if (isInString) {
                if (current == '\\') {
                    hasBackslash = !hasBackslash;
                } else {
                    hasBackslash = false;
                }
            }
            return prev;
        } else {
            return current;
        }
    }

    public char peek() {
        if (current == 0) {
            readInt();
        }
        return current;
    }

    public RCodeLookaheadStream getLookahead() {
        return sequenceIn.getLookahead();
    }

    public boolean hasNext() {
        return !hasReachedCommandEnd && sequenceIn.hasNext();
    }

    public void openCommand() {
        hasReachedCommandEnd = false;
        if (current == ';' || current == '\n') {
            read();
        }
    }

    public void closeCommand() {
        while (hasNext()) {
            read();
        }
    }

    public boolean isCommandOpen() {
        return !hasReachedCommandEnd;
    }

    public void skipSequence() {
        sequenceIn.closeCommandSequence();
    }

    public void unlock() {
        lock = false;
    }

    public boolean lock() {
        if (lock) {
            return false;
        } else {
            lock = true;
            return true;
        }

    }

    public boolean isLocked() {
        return lock;
    }

    public RCodeSequenceInStream getSequenceIn() {
        return sequenceIn;
    }
}

package com.wittsfamily.rcode.javareceiver.parsing;

public class RCodeInStream {
    private final RCodeSequenceInStream sequenceIn;
    private boolean hasReachedCommandEnd = false;
    private int current = -1;
    private boolean isInString = false;
    private boolean hasBackslash = false;
    private boolean lock = false;
    private boolean currentValid = false;
    private RCodeWrapperLookaheadStream stream = new RCodeWrapperLookaheadStream();

    private class RCodeWrapperLookaheadStream implements RCodeLookaheadStream {
        private boolean hasUsed = false;
        private RCodeLookaheadStream toWrap;

        public void reset(RCodeLookaheadStream toWrap) {
            hasUsed = false;
            this.toWrap = toWrap;
        }

        @Override
        public char read() {
            if (hasUsed || current == -1) {
                return toWrap.read();
            }
            hasUsed = true;
            return (char) current;
        }
    };

    public RCodeInStream(RCodeSequenceInStream sequenceIn) {
        this.sequenceIn = sequenceIn;
    }

    public char read() {
        int c = readInt();
        if (c == -1) {
            return (char) readInt();
        }
        return (char) c;
    }

    private int readInt() {
        if (!hasReachedCommandEnd) {
            int prev = current;
            if (sequenceIn.hasNextChar()) {
                current = sequenceIn.nextChar();
                currentValid = true;
                if (current == '\n' || ((current == '&' || current == '|') && !isInString)) {
                    hasReachedCommandEnd = true;
                    currentValid = false;
                } else if (current == '"') {
                    if (!hasBackslash) {
                        isInString = !isInString;
                    }
                } else if (isInString) {
                    if (current == '\\') {
                        hasBackslash = !hasBackslash;
                    } else {
                        hasBackslash = false;
                    }
                }
            } else {
                currentValid = false;
            }
            return prev;
        } else {
            return current;
        }
    }

    public char peek() {
        if (current == -1) {
            readInt();
        }
        return (char) current;
    }

    public RCodeLookaheadStream getLookahead() {
        stream.reset(sequenceIn.getLookahead());
        return stream;
    }

    public boolean hasNext() {
        return currentValid || !hasReachedCommandEnd && sequenceIn.hasNextChar();
    }

    public void openCommand() {
        hasReachedCommandEnd = false;
        currentValid = true;
        if (current == '&' || current == '|' || current == '\n') {
            read();
        }
    }

    public void closeCommand() {
        while (hasNext()) {
            read();
        }
        if (current != '\n' && current != '&' && current != '|') {
            current = sequenceIn.nextChar();
        }
    }

    public boolean isCommandOpen() {
        return !hasReachedCommandEnd;
    }

    public void skipSequence() {
        if (current != '\n') {
            sequenceIn.closeCommandSequence();
            hasReachedCommandEnd = true;
            currentValid = false;
            current = -1;
        }
    }

    public void skipToError() {
        while (hasNext() || current == '&') {
            if (current == '&') {
                openCommand();
            }
            read();
        }
        if (current != '\n' && current != '&' && current != '|') {
            current = sequenceIn.nextChar();
        }
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

    public void unOpen() {
        hasReachedCommandEnd = false;
        currentValid = true;
    }
}

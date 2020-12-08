package com.wittsfamily.rcode.javareceiver.instreams;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeLookaheadStream;

public class RCodeCommandInStream {
    private final RCodeSequenceInStream sequenceIn;
    private char current = 0;
    private boolean open = false;
    private boolean inString = false;
    private boolean backslash = false;

    public RCodeCommandInStream(RCodeSequenceInStream sequenceIn) {
        this.sequenceIn = sequenceIn;
    }

    public void reset() {
        current = 0;
        open = false;
        inString = false;
        backslash = false;
    }

    public void open() {
        if (!open) {
            open = true;
            readInternal();
        }
    }

    public void openWith(char c) {
        open = true;
        current = c;
    }

    public void close() {
        while (open) {
            readInternal();
        }
    }

    public char read() {
        if (open) {
            char prev = current;
            readInternal();
            return prev;
        } else {
            return current;
        }
    }

    public char peek() {
        return current;
    }

    public boolean hasNext() {
        return open;
    }

    private void readInternal() {
        current = sequenceIn.read();

        if (current == '\n' || (!inString && (current == '&' || current == '|'))) {
            open = false;
        } else if (current == '"' && !backslash) {
            inString = !inString;
        } else if (inString) {
            if (current == '\\') {
                backslash = !backslash;
            } else {
                backslash = false;
            }
        }
    }

    public void eatWhitespace() {
        while (open && (current == ' ' || current == '\t' || current == '\r')) {
            readInternal();
        }
    }

    public RCodeLookaheadStream getLookahead() {
        return new RCodeLookaheadStream() {
            private final RCodeLookaheadStream parent = sequenceIn.getChannelInStream().getLookahead();
            private int lookahead = 2;

            @Override
            public char read() {
                if (lookahead == 0) {
                    return parent.read();
                }
                if (lookahead-- == 2) {
                    return current;
                } else {
                    return sequenceIn.peek();
                }
            }
        };
    }
}

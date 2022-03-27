package org.zcode.javareceiver.instreams;

import org.zcode.javareceiver.Zchars;

public class ZcodeCommandInStream {
    private final ZcodeSequenceInStream sequenceIn;

    private char    current   = 0;
    private boolean open      = false;
    private boolean inString  = false;
    private boolean backslash = false;

    public ZcodeCommandInStream(final ZcodeSequenceInStream sequenceIn) {
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

    public void openWith(final char c) {
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
            final char prev = current;
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

        if (current == Zchars.EOL_SYMBOL.ch || (!inString && (current == Zchars.ANDTHEN_SYMBOL.ch || current == Zchars.ORELSE_SYMBOL.ch))) {
            open = false;
        } else if (current == Zchars.BIGFIELD_QUOTE_MARKER.ch && !backslash) {
            inString = !inString;
        } else if (inString) {
            if (current == Zchars.STRING_ESCAPE_SYMBOL.ch) {
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

    public ZcodeLookaheadStream getLookahead() {
        return new ZcodeLookaheadStream() {
            private final ZcodeLookaheadStream parent = sequenceIn.getChannelInStream().getLookahead();

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

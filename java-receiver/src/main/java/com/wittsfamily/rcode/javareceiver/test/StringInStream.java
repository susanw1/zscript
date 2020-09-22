package com.wittsfamily.rcode.javareceiver.test;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeLookaheadStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeSequenceInStream;

public class StringInStream implements RCodeSequenceInStream {
    private final String s;
    private int pos = 0;
    private boolean isAtStart = true;
    private boolean isOpen = false;

    public StringInStream(String s) {
        this.s = s;
    }

    @Override
    public char nextChar() {
        if (pos >= s.length()) {
            return '\n';
        }
        return s.charAt(pos++);
    }

    @Override
    public boolean hasNextChar() {
        return pos < s.length();
    }

    @Override
    public RCodeLookaheadStream getLookahead() {
        return new RCodeLookaheadStream() {
            int offset = 0;

            @Override
            public char read() {
                if (pos - 1 + offset >= s.length()) {
                    return '\n';
                }
                return s.charAt(pos - 1 + offset++);
            }
        };
    }

    @Override
    public boolean hasNextCommandSequence() {
        return isAtStart;
    }

    @Override
    public void openCommandSequence() {
        isAtStart = false;
        isOpen = true;
    }

    @Override
    public void closeCommandSequence() {
        if (isOpen) {
            isOpen = false;
            while (pos < s.length() && s.charAt(pos) != '\n') {
                if (s.charAt(pos) == 'C') {
                    int a = 1 / 0;
                }
                pos++;
            }
            pos++;
        }
    }

    @Override
    public boolean isCommandSequenceOpen() {
        return isOpen;
    }

}

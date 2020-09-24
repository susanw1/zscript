package com.wittsfamily.rcode.javareceiver_acceptancetests;

import java.util.Queue;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeLookaheadStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeSequenceInStream;

public class RCodeQueueSequenceInStream implements RCodeSequenceInStream {
    private final Queue<byte[]> messages;
    private boolean isOpen = false;
    private int pos = 0;

    public RCodeQueueSequenceInStream(Queue<byte[]> messages) {
        this.messages = messages;
    }

    @Override
    public char nextChar() {
        if (!isOpen || messages.isEmpty() || messages.peek().length <= pos || messages.peek()[pos] == '\n') {
            isOpen = false;
            return '\n';
        }
        return (char) messages.peek()[pos++];
    }

    @Override
    public boolean hasNextChar() {
        return isOpen && !messages.isEmpty() && pos < messages.peek().length && messages.peek()[pos] != '\n';
    }

    @Override
    public RCodeLookaheadStream getLookahead() {
        return new RCodeLookaheadStream() {
            private int offsetPos = 0;

            @Override
            public char read() {
                if (!isOpen || messages.isEmpty() || messages.peek().length <= pos + offsetPos || messages.peek()[pos + offsetPos] == '\n') {
                    return '\n';
                }
                return (char) messages.peek()[pos + offsetPos++];
            }
        };
    }

    @Override
    public boolean hasNextCommandSequence() {
        if (messages.isEmpty()) {
            return false;
        } else if (messages.size() > 1 || (pos == 0)) {
            return true;
        } else if (pos < messages.peek().length) {
            for (int i = pos; i < messages.peek().length; i++) {
                if (messages.peek()[i] == '\n') {
                    if (i != pos || messages.peek().length - i > 1) {
                        return true;
                    }
                }
            }
            return false;
        } else {
            return false;
        }
    }

    @Override
    public void openCommandSequence() {
        if (messages.peek()[pos] == '\n') {
            pos++;
        }
        if (pos >= messages.peek().length) {
            pos = 0;
            messages.poll();
        }
        isOpen = true;
    }

    @Override
    public void closeCommandSequence() {
        if (pos >= messages.peek().length) {
            pos = 0;
            messages.poll();
        } else {
            boolean found = false;
            for (int i = 0; i < messages.peek().length; i++) {
                if (messages.peek()[i] == '\n') {
                    pos = i;
                    found = true;
                    break;
                }
            }
            if (!found) {
                pos = 0;
                messages.poll();
            }
        }
    }

    @Override
    public boolean isCommandSequenceOpen() {
        return isOpen;
    }

}

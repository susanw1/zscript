package com.wittsfamily.rcode.javareceiver_acceptancetests;

import java.util.LinkedList;
import java.util.Queue;

import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeLookaheadStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeSequenceInStream;

public class RCodeQueueSequenceInStream implements RCodeChannelInStream {
    private final Queue<byte[]> messages = new LinkedList<>();
    private final RCodeSequenceInStream seqInStream = new RCodeSequenceInStream(this);
    private int pos = 0;

    public void addMessage(byte[] message) {
        messages.add(message);
    }

    @Override
    public int read() {
        if (messages.isEmpty() || messages.peek().length <= pos || messages.peek().length == pos + 1 && messages.peek()[pos] == '\n') {
            if (!messages.isEmpty()) {
                messages.poll();
            }
            pos = 0;
            return -1;
        }
        return (char) messages.peek()[pos++];
    }

    @Override
    public RCodeLookaheadStream getLookahead() {
        return new RCodeLookaheadStream() {
            private int offsetPos = 0;

            @Override
            public char read() {
                if (messages.isEmpty() || messages.peek().length <= pos + offsetPos) {
                    return '\n';
                }
                return (char) messages.peek()[pos + offsetPos++];
            }
        };
    }

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
    public RCodeSequenceInStream getSequenceInStream() {
        return seqInStream;
    }

}

package com.wittsfamily.rcode.javareceiver.instreams;

public class RCodeMarkerInStream {
    private final RCodeSequenceInStream sequenceIn;
    private boolean hasRead = false;

    public RCodeMarkerInStream(RCodeSequenceInStream sequenceIn) {
        this.sequenceIn = sequenceIn;
    }

    public void reset() {
        hasRead = false;
    }

    public char read() {
        if (hasRead) {
            sequenceIn.read();
        }
        hasRead = true;
        return sequenceIn.peek();
    }

    public char reread() {
        return sequenceIn.peek();
    }

    public void unread() {
        hasRead = false;
    }

    public boolean hasNext() {
        return sequenceIn.hasNext();
    }

    public void close() {
        sequenceIn.getCommandInStream().openWith(' ');
    }

    public void eatWhitespace() {
        char c = reread();
        while (c == ' ' || c == '\t' || c == '\r') {
            c = read();
        }
    }
}

package net.zscript.javareceiver.instreams;

public class ZcodeMarkerInStream {
    private final ZcodeSequenceInStream sequenceIn;

    private boolean hasRead = false;

    public ZcodeMarkerInStream(final ZcodeSequenceInStream sequenceIn) {
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
    }

    public void eatWhitespace() {
        char c = reread();
        while (c == ' ' || c == '\t' || c == '\r') {
            c = read();
        }
    }
}

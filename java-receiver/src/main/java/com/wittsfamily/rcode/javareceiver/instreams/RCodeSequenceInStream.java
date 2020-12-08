package com.wittsfamily.rcode.javareceiver.instreams;

public class RCodeSequenceInStream {
    private final RCodeChannelInStream channelIn;
    private final RCodeCommandInStream commandIn = new RCodeCommandInStream(this);
    private final RCodeMarkerInStream markerIn = new RCodeMarkerInStream(this);
    private char current = 0;
    private boolean open = false;
    private boolean lock = false;

    public RCodeSequenceInStream(RCodeChannelInStream channelIn) {
        this.channelIn = channelIn;
    }

    public void open() {
        if (!open) {
            commandIn.reset();
            open = true;
            readInternal();
        }
    }

    public void close() {
        commandIn.reset();
        if (open) {
            while (current != '\n') {
                readInternal();
            }
            open = false;
        }
    }

    public char read() {
        if (open) {
            char prev = current;
            readInternal();
            if (open && current == '\n') {
                open = false;
            }
            return prev;
        } else {
            return '\n';
        }
    }

    public char peek() {
        if (open) {
            return current;
        } else {
            return '\n';
        }
    }

    public boolean hasNext() {
        return open;
    }

    private void readInternal() {
        int next = channelIn.read();
        if (next == -1) {
            current = '\n';
            open = false;
        } else {
            current = (char) next;
        }
    }

    public void skipToError() {
        commandIn.close();
        while (commandIn.read() != '|' && open) {
            commandIn.open();
            commandIn.close();
        }
    }

    public RCodeChannelInStream getChannelInStream() {
        return channelIn;
    }

    public RCodeCommandInStream getCommandInStream() {
        return commandIn;
    }

    public RCodeMarkerInStream getMarkerInStream() {
        markerIn.reset();
        return markerIn;
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
}

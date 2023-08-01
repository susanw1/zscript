package net.zscript.javareceiver.instreams;

import net.zscript.javareceiver.Zchars;

public class ZcodeSequenceInStream {
    private final ZcodeChannelInStream channelIn;
    private final ZcodeCommandInStream commandIn = new ZcodeCommandInStream(this);
    private final ZcodeMarkerInStream  markerIn  = new ZcodeMarkerInStream(this);

    private char    current = 0;
    private boolean open    = false;
    private boolean lock    = false;

    public ZcodeSequenceInStream(final ZcodeChannelInStream channelIn) {
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
            while (current != Zchars.EOL_SYMBOL.ch) {
                readInternal();
            }
            open = false;
        }
    }

    public char read() {
        if (open) {
            final char prev = current;
            readInternal();
            if (open && current == Zchars.EOL_SYMBOL.ch) {
                open = false;
            }
            return prev;
        } else {
            return Zchars.EOL_SYMBOL.ch;
        }
    }

    public char peek() {
        if (open) {
            return current;
        } else {
            return Zchars.EOL_SYMBOL.ch;
        }
    }

    public boolean hasNext() {
        return open;
    }

    private void readInternal() {
        final int next = channelIn.read();
        if (next == -1) {
            current = Zchars.EOL_SYMBOL.ch;
            open = false;
        } else {
            current = (char) next;
        }
    }

    public void skipToError() {
        commandIn.close();
        while (commandIn.read() != Zchars.ORELSE_SYMBOL.ch && open) {
            commandIn.open();
            commandIn.close();
        }
    }

    public ZcodeChannelInStream getChannelInStream() {
        return channelIn;
    }

    public ZcodeCommandInStream getCommandInStream() {
        return commandIn;
    }

    public ZcodeMarkerInStream getMarkerInStream() {
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

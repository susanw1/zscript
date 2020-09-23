package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;

public class RCodeCommandSequence {
    private final RCode rcode;
    private final RCodeParameters params;
    private final RCodeCommandChannel channel;
    private RCodeCommandSlot first;
    private RCodeCommandSlot last;
    private boolean canBeParallel;
    private boolean isBroadcast;

    private boolean isFullyParsed;
    private boolean active;
    private boolean failed = false;
    private boolean empty = false;

    private RCodeInStream in = null;
    private RCodeOutStream out = null;
    private RCodeLockSet locks = null;

    public RCodeCommandSequence(RCode rcode, RCodeParameters params, RCodeCommandChannel channel) {
        this.rcode = rcode;
        this.params = params;
        this.channel = channel;
    }

    public RCodeInStream getInStream() {
        if (in == null) {
            in = channel.getInStream();
            in.lock();
        }
        return in;
    }

    public boolean canBeParallel() {
        return canBeParallel;
    }

    public boolean isBroadcast() {
        return isBroadcast;
    }

    public boolean isEmpty() {
        return empty;
    }

    public boolean hasFailed() {
        return failed;
    }

    public void unsetFailed() {
        this.failed = false;
    }

    public void releaseInStream() {
        if (in != null) {
            in.unlock();
        }
        channel.releaseInStream();
        in = null;
    }

    public RCodeOutStream getOutStream() {
        if (out == null) {
            out = channel.getOutStream();
            out.lock();
        }
        return out;
    }

    public void releaseOutStream() {
        out.unlock();
        channel.releaseOutStream();
        out = null;
    }

    public void setFullyParsed(boolean b) {
        isFullyParsed = b;
    }

    public boolean isFullyParsed() {
        return isFullyParsed;
    }

    public boolean isActive() {
        return active;
    }

    public void setActive() {
        this.active = true;
    }

    public void addLast(RCodeCommandSlot slot) {
        if (last != null) {
            last.next = slot;
            last = slot;
            if (locks != null) {
                slot.getCommand().setLocks(locks);
            }
        } else {
            last = slot;
            first = slot;
        }
    }

    public RCodeCommandSlot popFirst() {
        RCodeCommandSlot target = first;
        if (first == null) {
            return null;
        } else if (first.next != null) {
            first = first.next;
        } else {
            last = null;
            first = null;
        }
        return target;
    }

    public RCodeCommandSlot peekFirst() {
        return first;
    }

    public RCodeCommandChannel getChannel() {
        return channel;
    }

    public boolean hasParsed() {
        return first != null;
    }

    public void reset() {
        isBroadcast = false;
        canBeParallel = false;
        last = null;
        first = null;
        isFullyParsed = false;
        locks = null;
        active = false;
    }

    public void fail(RCodeResponseStatus status) {
        if (status != RCodeResponseStatus.CMD_FAIL) {
            for (RCodeCommandSlot current = first; current != null; current = current.next) {
                current.reset();
            }
            if (in != null) {
                in.skipSequence();
            }
            failed = true;
            first = null;
            last = null;
            locks = null;
        }
    }

    public boolean canLock() {
        if (locks == null) {
            locks = new RCodeLockSet(params);
            for (RCodeCommandSlot slot = first; slot != null; slot = slot.next) {
                if (slot.getCommand() != null) {
                    slot.getCommand().setLocks(locks);
                }
            }
        }
        return rcode.canLock(locks);
    }

    public void lock() {
        if (locks == null) {
            locks = new RCodeLockSet(params);
            for (RCodeCommandSlot slot = first; slot != null; slot = slot.next) {
                slot.getCommand().setLocks(locks);
            }
        }
        rcode.lock(locks);
    }

    public void unlock() {
        if (locks != null) {
            rcode.unlock(locks);
            locks = null;
        }
    }

    public boolean parseFlags() {
        in.openCommand();
        if (RCodeParser.shouldEatWhitespace(in)) {
            RCodeParser.eatWhitespace(in);
        }
        if (in.peek() == '#') {
            in.skipSequence();
            return false;
        }
        if (in.peek() == '*') {
            in.read();
            isBroadcast = true;
            if (RCodeParser.shouldEatWhitespace(in)) {
                RCodeParser.eatWhitespace(in);
            }
        }
        if (in.peek() == '%') {
            in.read();
            canBeParallel = true;
            if (RCodeParser.shouldEatWhitespace(in)) {
                RCodeParser.eatWhitespace(in);
            }
        }
        if (!in.getSequenceIn().hasNextChar()) {
            empty = true;
            canBeParallel = true;
            isFullyParsed = true;
        }

        in.unOpen();
        return true;
    }
}

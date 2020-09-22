package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;

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

    public boolean hasFailed() {
        return failed;
    }

    public void unsetFailed() {
        this.failed = false;
    }

    public void releaseInStream() {
        in.unlock();
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

    public void setBroadcast() {
        isBroadcast = true;
    }

    public void setParallel() {
        canBeParallel = true;
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

    public void fail() {
        for (RCodeCommandSlot current = first; current != null; current = current.next) {
            current.reset();
        }
        if (in != null) {
            in.skipSequence();
        }
        failed = true;
        last = first;
        first.next = null;
        locks = null;
    }

    public boolean canLock() {
        if (locks == null) {
            locks = new RCodeLockSet(params);
            for (RCodeCommandSlot slot = first; slot != null; slot = slot.next) {
                if (slot.getCommand() != null) {
                    slot.getCommand().setLocks(locks);
                }
            }
            channel.setLocks(locks);
        }
        return rcode.canLock(locks);
    }

    public void lock() {
        if (locks == null) {
            locks = new RCodeLockSet(params);
            for (RCodeCommandSlot slot = first; slot != null; slot = slot.next) {
                slot.getCommand().setLocks(locks);
            }
            channel.setLocks(locks);
        }
        rcode.lock(locks);
    }

    public void unlock() {
        if (locks != null) {
            rcode.unlock(locks);
            locks = null;
        }
    }
}

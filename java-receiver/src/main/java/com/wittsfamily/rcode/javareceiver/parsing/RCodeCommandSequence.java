package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeMarkerInStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeSequenceInStream;

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

    private RCodeSequenceInStream in = null;
    private RCodeOutStream out = null;
    private RCodeLockSet locks = null;

    public RCodeCommandSequence(RCode rcode, RCodeParameters params, RCodeCommandChannel channel) {
        this.rcode = rcode;
        this.params = params;
        this.channel = channel;
    }

    public RCodeSequenceInStream acquireInStream() {
        if (in == null) {
            in = channel.getInStream().getSequenceInStream();
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

    public RCodeOutStream acquireOutStream() {
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

    public boolean canContinueParse() {
        return active && !isFullyParsed && !failed;
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
        empty = false;
    }

    public boolean fail(RCodeResponseStatus status) {
        if (status != RCodeResponseStatus.CMD_FAIL) {
            RCodeCommandSlot next = null;
            for (RCodeCommandSlot current = first; current != null; current = next) {
                next = current.next;
                current.reset();
            }
            if (in != null) {
                in.close();
            }
            if (!isFullyParsed) {
                failed = true;
                isFullyParsed = true;
            }
            first = null;
            last = null;
            locks = null;
            return false;
        } else {
            boolean found = false;
            RCodeCommandSlot next = null;
            for (RCodeCommandSlot current = first; current != null; current = next) {
                next = current.next;
                if (current.getEnd() == '|') {
                    found = true;
                }
                current.reset();
                if (found) {
                    first = next;
                    break;
                }
            }
            if (!found || first == null) {
                first = null;
                last = null;
                if (isFullyParsed) {
                    locks = null;
                    return false;
                } else {
                    in.skipToError();
                    if (in.peek() == '\n') {
                        if (!isFullyParsed) {
                            failed = true;
                            isFullyParsed = true;
                        }
                        locks = null;
                        return false;
                    }
                    return true;
                }
            } else {
                return true;
            }
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

    public boolean parseRCodeMarkers() {
        RCodeMarkerInStream mIn = in.getMarkerInStream();
        mIn.eatWhitespace();
        mIn.read();
        if (mIn.reread() == '#') {
            mIn.close();
            in.close();
            return false;
        }
        if (mIn.reread() == '*') {
            mIn.read();
            isBroadcast = true;
            mIn.eatWhitespace();
        }
        if (mIn.reread() == '%') {
            mIn.read();
            canBeParallel = true;
            mIn.eatWhitespace();
        }
        if (!mIn.hasNext()) {
            empty = true;
            canBeParallel = true;
            isFullyParsed = true;
        }

        mIn.close();
        return true;
    }
}

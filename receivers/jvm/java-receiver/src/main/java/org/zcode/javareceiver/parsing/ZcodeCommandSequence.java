package org.zcode.javareceiver.parsing;

import org.zcode.javareceiver.Zchars;
import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeLockSet;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.ZcodeResponseStatus;
import org.zcode.javareceiver.instreams.ZcodeMarkerInStream;
import org.zcode.javareceiver.instreams.ZcodeSequenceInStream;

public class ZcodeCommandSequence {
    private final Zcode               zcode;
    private final ZcodeParameters     params;
    private final ZcodeCommandChannel channel;
    private ZcodeCommandSlot          first;
    private ZcodeCommandSlot          last;
    private boolean                   canBeParallel;
    private boolean                   isBroadcast;

    private boolean isFullyParsed;
    private boolean active;
    private boolean failed = false;
    private boolean empty  = false;

    private ZcodeSequenceInStream in    = null;
    private ZcodeOutStream        out   = null;
    private ZcodeLockSet          locks = null;

    public ZcodeCommandSequence(final Zcode zcode, final ZcodeParameters params, final ZcodeCommandChannel channel) {
        this.zcode = zcode;
        this.params = params;
        this.channel = channel;
    }

    public ZcodeSequenceInStream acquireInStream() {
        if (in == null) {
            in = channel.acquireInStream().getSequenceInStream();
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

    public ZcodeOutStream acquireOutStream() {
        if (out == null) {
            out = channel.acquireOutStream();
            out.lock();
        }
        return out;
    }

    public void releaseOutStream() {
        out.unlock();
        channel.releaseOutStream();
        out = null;
    }

    public void setFullyParsed(final boolean b) {
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

    public void addLast(final ZcodeCommandSlot slot) {
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

    public ZcodeCommandSlot popFirst() {
        final ZcodeCommandSlot target = first;
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

    public ZcodeCommandSlot peekFirst() {
        return first;
    }

    public ZcodeCommandChannel getChannel() {
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

    public boolean fail(final ZcodeResponseStatus status) {
        if (status != ZcodeResponseStatus.CMD_FAIL) {
            ZcodeCommandSlot next = null;
            for (ZcodeCommandSlot current = first; current != null; current = next) {
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
            boolean          found = false;
            ZcodeCommandSlot next  = null;
            for (ZcodeCommandSlot current = first; current != null; current = next) {
                next = current.next;
                if (current.getEnd() == Zchars.ORELSE_SYMBOL.ch) {
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
                    if (in.peek() == Zchars.EOL_SYMBOL.ch) {
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
            locks = new ZcodeLockSet(params);
            for (ZcodeCommandSlot slot = first; slot != null; slot = slot.next) {
                if (slot.getCommand() != null) {
                    slot.getCommand().setLocks(locks);
                }
            }
        }
        return zcode.canLock(locks);
    }

    public void lock() {
        if (locks == null) {
            locks = new ZcodeLockSet(params);
            for (ZcodeCommandSlot slot = first; slot != null; slot = slot.next) {
                slot.getCommand().setLocks(locks);
            }
        }
        zcode.lock(locks);
    }

    public void unlock() {
        if (locks != null) {
            zcode.unlock(locks);
            locks = null;
        }
    }

    public boolean parseZcodeMarkers() {
        final ZcodeMarkerInStream mIn = in.getMarkerInStream();
        mIn.eatWhitespace();
        mIn.read();
        if (mIn.reread() == Zchars.DEBUG_PREFIX.ch) {
            mIn.close();
            in.close();
            return false;
        }
        if (mIn.reread() == Zchars.BROADCAST_PREFIX.ch) {
            mIn.read();
            isBroadcast = true;
            mIn.eatWhitespace();
        }
        if (mIn.reread() == Zchars.PARALLEL_PREFIX.ch) {
            mIn.read();
            canBeParallel = true;
            mIn.eatWhitespace();
        }
        if (mIn.reread() == Zchars.EOL_SYMBOL.ch) {
            empty = true;
            canBeParallel = true;
            isFullyParsed = true;
        }

        mIn.close();
        return true;
    }
}

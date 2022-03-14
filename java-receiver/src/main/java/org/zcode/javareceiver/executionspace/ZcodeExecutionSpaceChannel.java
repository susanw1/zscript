package org.zcode.javareceiver.executionspace;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeLockSet;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.instreams.ZcodeChannelInStream;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;

public class ZcodeExecutionSpaceChannel implements ZcodeCommandChannel {
    private final Zcode                 zcode;
    private final ZcodeExecutionSpace   space;
    private final ZcodeCommandSequence  sequence;
    private int                         position   = 0;
    private int                         delayTimer = 0;
    private ZcodeExecutionSpaceInStream in         = null;
    private ZcodeOutStream              out        = null;
    private final ZcodeLockSet          locks;

    public ZcodeExecutionSpaceChannel(final ZcodeParameters params, final Zcode zcode, final ZcodeExecutionSpace space) {
        this.locks = new ZcodeLockSet(params);
        this.zcode = zcode;
        this.space = space;
        this.sequence = new ZcodeCommandSequence(zcode, params, this);
    }

    @Override
    public ZcodeChannelInStream acquireInStream() {
        if (in == null) {
            in = space.acquireInStream(position);
        }
        return in;
    }

    @Override
    public boolean hasInStream() {
        return in != null;
    }

    @Override
    public ZcodeOutStream acquireOutStream() {
        if (out == null) {
            out = space.acquireOutStream();
        }
        return out;
    }

    @Override
    public boolean hasOutStream() {
        return out != null;
    }

    @Override
    public boolean hasCommandSequence() {
        final boolean has = space.isRunning() && space.hasInStream() && space.hasOutStream() && in == null && out == null && delayTimer >= space.getDelay();
        delayTimer++;
        return has;
    }

    @Override
    public ZcodeCommandSequence getCommandSequence() {
        return sequence;
    }

    @Override
    public boolean isPacketBased() {
        return true;
    }

    @Override
    public void releaseInStream() {
        delayTimer = 0;
        if (in != null) {
            position = in.getPosition() + 1;
            if (position >= space.getLength()) {
                position = 0;
            }
            space.releaseInStream(in);
        }
        in = null;
    }

    @Override
    public void releaseOutStream() {
        space.releaseOutStream((ZcodeExecutionSpaceOut) out);
        out = null;
    }

    @Override
    public void setAsNotificationChannel() {
        throw new UnsupportedOperationException("Execution Space cannot be notification channel");
    }

    @Override
    public void releaseFromNotificationChannel() {
        throw new UnsupportedOperationException("Execution Space cannot be notification channel");
    }

    @Override
    public void setAsDebugChannel() {
        throw new UnsupportedOperationException("Execution Space cannot be notification channel");
    }

    @Override
    public void releaseFromDebugChannel() {
        throw new UnsupportedOperationException("Execution Space cannot be notification channel");
    }

    @Override
    public void lock() {
        zcode.lock(locks);
    }

    @Override
    public boolean canLock() {
        return zcode.canLock(locks);
    }

    @Override
    public void unlock() {
        zcode.unlock(locks);
    }

}

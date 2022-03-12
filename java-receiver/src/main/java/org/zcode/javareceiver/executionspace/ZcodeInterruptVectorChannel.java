package org.zcode.javareceiver.executionspace;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeBusInterrupt;
import org.zcode.javareceiver.ZcodeLockSet;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.instreams.ZcodeChannelInStream;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;

public class ZcodeInterruptVectorChannel implements ZcodeCommandChannel {
    private final Zcode                       zcode;
    private final ZcodeExecutionSpace         space;
    private final ZcodeInterruptVectorManager vectorManager;
    private final ZcodeCommandSequence        sequence;
    private ZcodeBusInterrupt                 interrupt = null;
    private ZcodeExecutionSpaceInStream       in        = null;
    private final ZcodeLockSet                locks;

    public ZcodeInterruptVectorChannel(final ZcodeExecutionSpace space, final ZcodeInterruptVectorManager vectorManager, final Zcode zcode, final ZcodeParameters params) {
        this.zcode = zcode;
        this.space = space;
        this.vectorManager = vectorManager;
        this.sequence = new ZcodeCommandSequence(zcode, params, this);
        this.locks = new ZcodeLockSet(params);
    }

    @Override
    public ZcodeChannelInStream acquireInStream() {
        if (interrupt == null) {
            interrupt = vectorManager.takeInterrupt();
        }
        if (in == null) {
            in = space.acquireInStream(vectorManager.findVector(interrupt));
        }
        return in;
    }

    @Override
    public boolean hasInStream() {
        return in != null;
    }

    @Override
    public ZcodeOutStream acquireOutStream() {
        return vectorManager.getOut();
    }

    @Override
    public boolean hasOutStream() {
        return true;
    }

    @Override
    public boolean hasCommandSequence() {
        return interrupt == null && vectorManager.hasInterruptSource() && space.hasInStream();
    }

    @Override
    public ZcodeCommandSequence getCommandSequence() {
        return sequence;
    }

    @Override
    public boolean isPacketBased() {
        return false;
    }

    @Override
    public void releaseInStream() {
        if (in != null) {
            space.releaseInStream(in);
        }
        in = null;
    }

    @Override
    public void releaseOutStream() {
        interrupt = null;
    }

    public ZcodeBusInterrupt getInterrupt() {
        return interrupt;
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

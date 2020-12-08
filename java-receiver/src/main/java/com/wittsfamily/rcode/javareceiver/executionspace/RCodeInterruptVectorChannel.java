package com.wittsfamily.rcode.javareceiver.executionspace;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeBusInterrupt;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;

public class RCodeInterruptVectorChannel implements RCodeCommandChannel {
    private final RCode rcode;
    private final RCodeExecutionSpace space;
    private final RCodeInterruptVectorManager vectorManager;
    private final RCodeCommandSequence sequence;
    private RCodeBusInterrupt interrupt = null;
    private RCodeExecutionSpaceInStream in = null;
    private RCodeLockSet locks;

    public RCodeInterruptVectorChannel(RCodeExecutionSpace space, RCodeInterruptVectorManager vectorManager, RCode r, RCodeParameters params) {
        this.rcode = r;
        this.space = space;
        this.vectorManager = vectorManager;
        this.sequence = new RCodeCommandSequence(r, params, this);
        this.locks = new RCodeLockSet(params);
    }

    @Override
    public RCodeChannelInStream getInStream() {
        if (interrupt == null) {
            interrupt = vectorManager.takeInterrupt();
        }
        if (in == null) {
            in = space.acquireInStream(vectorManager.findVector(interrupt));
        }
        return in;
    }

    @Override
    public RCodeOutStream getOutStream() {
        return vectorManager.getOut();
    }

    @Override
    public boolean hasCommandSequence() {
        return interrupt == null && vectorManager.hasInterruptSource() && space.hasInStream();
    }

    @Override
    public RCodeCommandSequence getCommandSequence() {
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

    public RCodeBusInterrupt getInterrupt() {
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
        rcode.lock(locks);
    }

    @Override
    public boolean canLock() {
        return rcode.canLock(locks);
    }

    @Override
    public void unlock() {
        rcode.unlock(locks);
    }
}

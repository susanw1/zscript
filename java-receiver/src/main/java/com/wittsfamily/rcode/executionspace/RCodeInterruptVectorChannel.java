package com.wittsfamily.rcode.executionspace;

import com.wittsfamily.rcode.RCodeBusInterrupt;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeInStream;

public class RCodeInterruptVectorChannel implements RCodeCommandChannel {
    private final RCodeExecutionSpace space;
    private final RCodeInterruptVectorManager vectorManager;
    private final RCodeCommandSequence sequence;
    private RCodeBusInterrupt interrupt = null;
    private RCodeInStream in = null;

    private RCodeInterruptVectorChannel(RCodeExecutionSpace space, RCodeInterruptVectorManager vectorManager, RCodeCommandSequence sequence) {
        this.space = space;
        this.vectorManager = vectorManager;
        this.sequence = sequence;
    }

    @Override
    public RCodeInStream getInStream() {
        if (interrupt == null) {
            interrupt = vectorManager.takeInterrupt();
        }
        if (in == null) {
            in = new RCodeInStream(space.acquireInStream(vectorManager.findVector(interrupt)));
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
        space.releaseInStream((RCodeExecutionSpaceSequenceIn) in.getSequenceIn());
        in = null;
    }

    @Override
    public void releaseOutStream() {
    }

    public RCodeBusInterrupt getInterrupt() {
        return interrupt;
    }

    @Override
    public void setAsNotificationChannel() {
        throw new UnsupportedOperationException("Interrupt Vector Channel cannot be notification channel");
    }
}

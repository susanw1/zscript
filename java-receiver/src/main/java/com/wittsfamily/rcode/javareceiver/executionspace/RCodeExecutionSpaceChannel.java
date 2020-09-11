package com.wittsfamily.rcode.javareceiver.executionspace;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeInStream;

public class RCodeExecutionSpaceChannel implements RCodeCommandChannel {
    private final RCodeExecutionSpace space;
    private final RCodeCommandSequence sequence;
    private int position = 0;
    private RCodeInStream in = null;
    private RCodeOutStream out = null;

    public RCodeExecutionSpaceChannel(RCodeParameters params, RCode rcode, RCodeExecutionSpace space) {
        this.space = space;
        this.sequence = new RCodeCommandSequence(rcode, params, this);
    }

    @Override
    public RCodeInStream getInStream() {
        if (in == null) {
            in = new RCodeInStream(space.acquireInStream(position));
        }
        return in;
    }

    @Override
    public RCodeOutStream getOutStream() {
        if (out == null) {
            out = space.acquireOutStream();
        }
        return out;
    }

    @Override
    public boolean hasCommandSequence() {
        boolean has = space.isRunning() && space.hasInStream() && space.hasOutStream() && in == null && out == null;
        if (has) {
            in = new RCodeInStream(space.acquireInStream(position));
            out = space.acquireOutStream();
        }
        return has;
    }

    @Override
    public RCodeCommandSequence getCommandSequence() {
        return sequence;
    }

    @Override
    public boolean isPacketBased() {
        return true;
    }

    @Override
    public void releaseInStream() {
        position = ((RCodeExecutionSpaceSequenceIn) in.getSequenceIn()).getPosition();
        space.releaseInStream((RCodeExecutionSpaceSequenceIn) in.getSequenceIn());
        in = null;
    }

    @Override
    public void releaseOutStream() {
        space.releaseOutStream((RCodeExecutionSpaceOut) out);
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
    public void setLocks(RCodeLockSet locks) {
        // TODO Auto-generated method stub
    }

}

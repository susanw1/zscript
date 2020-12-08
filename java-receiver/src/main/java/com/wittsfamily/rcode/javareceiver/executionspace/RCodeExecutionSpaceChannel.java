package com.wittsfamily.rcode.javareceiver.executionspace;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;

public class RCodeExecutionSpaceChannel implements RCodeCommandChannel {
    private final RCode rcode;
    private final RCodeExecutionSpace space;
    private final RCodeCommandSequence sequence;
    private int position = 0;
    private int delayTimer = 0;
    private RCodeExecutionSpaceInStream in = null;
    private RCodeOutStream out = null;
    private RCodeLockSet locks;

    public RCodeExecutionSpaceChannel(RCodeParameters params, RCode rcode, RCodeExecutionSpace space) {
        this.locks = new RCodeLockSet(params);
        this.rcode = rcode;
        this.space = space;
        this.sequence = new RCodeCommandSequence(rcode, params, this);
    }

    @Override
    public RCodeChannelInStream getInStream() {
        if (in == null) {
            in = space.acquireInStream(position);
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
        boolean has = space.isRunning() && space.hasInStream() && space.hasOutStream() && in == null && out == null && delayTimer >= space.getDelay();
        if (has) {
            in = space.acquireInStream(position);
            out = space.acquireOutStream();
        }
        delayTimer++;
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

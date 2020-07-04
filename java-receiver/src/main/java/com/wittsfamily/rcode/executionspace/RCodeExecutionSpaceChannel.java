package com.wittsfamily.rcode.executionspace;

import com.wittsfamily.rcode.RCode;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeInStream;

public class RCodeExecutionSpaceChannel implements RCodeCommandChannel {
    private final RCodeExecutionSpace space;
    private final RCodeCommandSequence sequence;
    private int position;
    private RCodeInStream in;
    private RCodeOutStream out;

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
        boolean has = space.hasInStream() && space.hasOutStream() && in == null && out == null;
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
        return false;
    }

    @Override
    public void releaseInStream() {
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

}

package com.wittsfamily.rcode.test;

import com.wittsfamily.rcode.RCode;
import com.wittsfamily.rcode.RCodeLockSet;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeInStream;

public class DirectCommandChannel implements RCodeCommandChannel {
    private final StringInStream s;
    private final RCodeOutStream out;
    private final RCodeCommandSequence seq;
    private final boolean isPacket;
    private int sequenceNumber;

    public DirectCommandChannel(RCodeParameters params, RCode rcode, String str, RCodeOutStream out, boolean isPacket, int sequenceNumber) {
        this.s = new StringInStream(str);
        this.seq = new RCodeCommandSequence(rcode, params, this);
        this.out = out;
        this.isPacket = isPacket;
        this.sequenceNumber = sequenceNumber;
    }

    @Override
    public RCodeInStream getInStream() {
        return new RCodeInStream(s);
    }

    @Override
    public RCodeOutStream getOutStream() {
        sequenceNumber--;
        return out;
    }

    @Override
    public boolean hasCommandSequence() {
        return sequenceNumber > 0;
    }

    @Override
    public RCodeCommandSequence getCommandSequence() {
        return seq;
    }

    @Override
    public boolean isPacketBased() {
        return isPacket;
    }

    @Override
    public void releaseInStream() {

    }

    @Override
    public void releaseOutStream() {

    }

    @Override
    public void setAsNotificationChannel() {
    }

    @Override
    public void setLocks(RCodeLockSet locks) {

    }

}

package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;

public class DirectCommandChannel implements RCodeCommandChannel {
    private final StringInStream s;
    private final RCodeOutStream out;
    private final RCodeCommandSequence seq;
    private final boolean isPacket;
    public int sequenceNumber;

    public DirectCommandChannel(RCodeParameters params, RCode rcode, String str, RCodeOutStream out, boolean isPacket, int sequenceNumber) {
        this.s = new StringInStream(str);
        this.seq = new RCodeCommandSequence(rcode, params, this);
        this.out = out;
        this.isPacket = isPacket;
        this.sequenceNumber = sequenceNumber;
    }

    @Override
    public RCodeChannelInStream acquireInStream() {
        return s;
    }

    @Override
    public boolean hasInStream() {
        return true;
    }

    @Override
    public RCodeOutStream acquireOutStream() {
        return out;
    }

    @Override
    public boolean hasOutStream() {
        return true;
    }

    @Override
    public boolean hasCommandSequence() {
        return s.hasNext();
    }

    @Override
    public RCodeCommandSequence getCommandSequence() {
        sequenceNumber--;
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
    public void releaseFromNotificationChannel() {
    }

    @Override
    public void setAsDebugChannel() {
    }

    @Override
    public void releaseFromDebugChannel() {
    }

    @Override
    public void lock() {
    }

    @Override
    public boolean canLock() {
        return true;
    }

    @Override
    public void unlock() {
    }

}

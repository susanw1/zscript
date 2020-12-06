package com.wittsfamily.rcode.javareceiver.test;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeParameters;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeInStream;

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
    public RCodeInStream getInStream() {
        return new RCodeInStream(s);
    }

    @Override
    public RCodeOutStream getOutStream() {
        return out;
    }

    @Override
    public boolean hasCommandSequence() {
        return s.hasNextChar();
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

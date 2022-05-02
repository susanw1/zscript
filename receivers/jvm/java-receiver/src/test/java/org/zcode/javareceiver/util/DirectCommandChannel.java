package org.zcode.javareceiver.util;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeParameters;
import org.zcode.javareceiver.instreams.ZcodeChannelInStream;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;

public class DirectCommandChannel implements ZcodeCommandChannel {
    private final StringInStream       s;
    private final ZcodeOutStream       out;
    private final ZcodeCommandSequence seq;
    private final boolean              isPacket;
    public int                         sequenceNumber;

    public DirectCommandChannel(final ZcodeParameters params, final Zcode zcode, final String str, final ZcodeOutStream out, final boolean isPacket, final int sequenceNumber) {
        this.s = new StringInStream(str);
        this.seq = new ZcodeCommandSequence(zcode, params, this);
        this.out = out;
        this.isPacket = isPacket;
        this.sequenceNumber = sequenceNumber;
    }

    @Override
    public ZcodeChannelInStream acquireInStream() {
        return s;
    }

    @Override
    public boolean hasInStream() {
        return true;
    }

    @Override
    public ZcodeOutStream acquireOutStream() {
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
    public ZcodeCommandSequence getCommandSequence() {
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

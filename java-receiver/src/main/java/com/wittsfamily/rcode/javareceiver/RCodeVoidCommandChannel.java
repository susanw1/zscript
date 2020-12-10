package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;

public class RCodeVoidCommandChannel implements RCodeCommandChannel {

    @Override
    public RCodeChannelInStream acquireInStream() {
        return new RCodeVoidInStream();
    }

    @Override
    public boolean hasInStream() {
        return true;
    }

    @Override
    public RCodeOutStream acquireOutStream() {
        return new RCodeVoidOutStream();
    }

    @Override
    public boolean hasOutStream() {
        return true;
    }

    @Override
    public boolean hasCommandSequence() {
        return false;
    }

    @Override
    public RCodeCommandSequence getCommandSequence() {
        return null;
    }

    @Override
    public boolean isPacketBased() {
        return false;
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

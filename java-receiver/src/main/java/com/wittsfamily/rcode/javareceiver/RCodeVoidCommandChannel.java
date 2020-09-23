package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeInStream;

public class RCodeVoidCommandChannel implements RCodeCommandChannel {

    @Override
    public RCodeInStream getInStream() {
        return new RCodeInStream(new RCodeVoidInStream());
    }

    @Override
    public RCodeOutStream getOutStream() {
        return new RCodeVoidOutStream();
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

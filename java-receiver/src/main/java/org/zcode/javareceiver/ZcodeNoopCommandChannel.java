package org.zcode.javareceiver;

import org.zcode.javareceiver.instreams.ZcodeChannelInStream;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;

public class ZcodeNoopCommandChannel implements ZcodeCommandChannel {

    @Override
    public ZcodeChannelInStream acquireInStream() {
        return new ZcodeNoopInStream();
    }

    @Override
    public boolean hasInStream() {
        return true;
    }

    @Override
    public ZcodeOutStream acquireOutStream() {
        return new ZcodeNoopOutStream();
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
    public ZcodeCommandSequence getCommandSequence() {
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

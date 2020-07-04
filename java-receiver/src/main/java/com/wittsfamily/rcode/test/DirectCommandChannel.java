package com.wittsfamily.rcode.test;

import com.wittsfamily.rcode.RCode;
import com.wittsfamily.rcode.RCodeLockSet;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeParameters;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeInStream;

public class DirectCommandChannel implements RCodeCommandChannel {
    private final StringInStream s = new StringInStream();
    private final RCodeCommandSequence seq;

    public DirectCommandChannel(RCode rcode) {
        this.seq = new RCodeCommandSequence(rcode, new RCodeParameters(), this);
    }

    @Override
    public RCodeInStream getInStream() {
        return new RCodeInStream(s);
    }

    @Override
    public RCodeOutStream getOutStream() {
        return new PrintingOutStream();
    }

    @Override
    public boolean hasCommandSequence() {
        return true;
    }

    @Override
    public RCodeCommandSequence getCommandSequence() {
        return seq;
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
    public void setLocks(RCodeLockSet locks) {

    }

}

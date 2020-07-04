package com.wittsfamily.rcode.parsing;

import com.wittsfamily.rcode.RCodeLockSet;
import com.wittsfamily.rcode.RCodeOutStream;

public interface RCodeCommandChannel {
    RCodeInStream getInStream();

    RCodeOutStream getOutStream();

    boolean hasCommandSequence();

    RCodeCommandSequence getCommandSequence();

    boolean isPacketBased();

    void releaseInStream();

    void releaseOutStream();

    void setAsNotificationChannel();

    void setLocks(RCodeLockSet locks);
}

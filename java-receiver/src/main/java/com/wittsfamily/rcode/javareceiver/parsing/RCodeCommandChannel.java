package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;

public interface RCodeCommandChannel {
    RCodeInStream getInStream();

    RCodeOutStream getOutStream();

    boolean hasCommandSequence();

    RCodeCommandSequence getCommandSequence();

    boolean isPacketBased();

    void releaseInStream();

    void releaseOutStream();

    void setAsNotificationChannel();

    void releaseFromNotificationChannel();

    void setAsDebugChannel();

    void releaseFromDebugChannel();

    void setLocks(RCodeLockSet locks);
}

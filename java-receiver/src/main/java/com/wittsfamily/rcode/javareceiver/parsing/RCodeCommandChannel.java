package com.wittsfamily.rcode.javareceiver.parsing;

import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.instreams.RCodeChannelInStream;

public interface RCodeCommandChannel {
    RCodeChannelInStream acquireInStream();

    boolean hasInStream();

    RCodeOutStream acquireOutStream();

    boolean hasOutStream();

    boolean hasCommandSequence();

    RCodeCommandSequence getCommandSequence();

    boolean isPacketBased();

    void releaseInStream();

    void releaseOutStream();

    void setAsNotificationChannel();

    void releaseFromNotificationChannel();

    void setAsDebugChannel();

    void releaseFromDebugChannel();

    void lock();

    boolean canLock();

    void unlock();
}

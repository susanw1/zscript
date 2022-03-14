package org.zcode.javareceiver.parsing;

import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.instreams.ZcodeChannelInStream;

public interface ZcodeCommandChannel {
    ZcodeChannelInStream acquireInStream();

    boolean hasInStream();

    ZcodeOutStream acquireOutStream();

    boolean hasOutStream();

    boolean hasCommandSequence();

    ZcodeCommandSequence getCommandSequence();

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

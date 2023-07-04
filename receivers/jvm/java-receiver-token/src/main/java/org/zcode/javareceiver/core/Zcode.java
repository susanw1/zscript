package org.zcode.javareceiver.core;

import java.util.ArrayList;
import java.util.List;

import org.zcode.javareceiver.execution.ZcodeExecutor;

public class Zcode {
    private final ZcodeLocks         locks    = new ZcodeLocks();
    private final List<ZcodeChannel> channels = new ArrayList<>();
    private final ZcodeExecutor      executor;

    public Zcode() {
        executor = new ZcodeExecutor(this);
    }

    public void addChannel(ZcodeChannel ch) {
        channels.add(ch);
    }

    public boolean lock(ZcodeLockSet l) {
        return locks.lock(l);
    }

    public boolean canLock(ZcodeLockSet l) {
        return locks.canLock(l);
    }

    public void unlock(ZcodeLockSet l) {
        locks.unlock(l);
    }

    public void progress() {
        for (ZcodeChannel channel : channels) {
            channel.moveAlong();
        }
        executor.progress(channels);
    }

}

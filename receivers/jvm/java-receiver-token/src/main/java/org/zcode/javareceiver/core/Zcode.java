package org.zcode.javareceiver.core;

import java.util.ArrayList;
import java.util.List;

import org.zcode.javareceiver.execution.ZcodeExecutor;
import org.zcode.javareceiver.modules.ZcodeModule;
import org.zcode.javareceiver.modules.ZcodeModuleFinder;

public class Zcode {
    private final ZcodeModuleFinder  finder   = new ZcodeModuleFinder();
    private final ZcodeLocks         locks    = new ZcodeLocks();
    private final List<ZcodeChannel> channels = new ArrayList<>();
    private final ZcodeExecutor      executor;

    public Zcode() {
        executor = new ZcodeExecutor(this);
    }

    public void addModule(ZcodeModule m) {
        finder.addModule(m);
    }

    public void addChannel(ZcodeChannel ch) {
        ch.setChannelIndex((byte) channels.size());
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

    public List<ZcodeChannel> getChannels() {
        return channels;
    }

    public ZcodeModuleFinder getModuleFinder() {
        return finder;
    }

}

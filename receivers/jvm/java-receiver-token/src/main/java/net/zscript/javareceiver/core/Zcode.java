package net.zscript.javareceiver.core;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.execution.ZcodeExecutor;
import net.zscript.javareceiver.modules.ZcodeModule;
import net.zscript.javareceiver.modules.ZcodeModuleRegistry;
import net.zscript.javareceiver.notifications.ZcodeNotificationSource;

public class Zcode {
    private final ZcodeModuleRegistry moduleRegistry = new ZcodeModuleRegistry();
    private final ZcodeLocks          locks          = new ZcodeLocks();
    private final List<ZcodeChannel>  channels       = new ArrayList<>();
    private final List<ActionSource>  sources        = new ArrayList<>();
    private final ZcodeExecutor       executor;

    private ZcodeOutStream notificationOutStream = new ZcodeAbstractOutStream() {
        private boolean open = false;

        @Override
        public void open() {
            open = true;
        }

        @Override
        public void close() {
            open = false;

        }

        @Override
        public boolean isOpen() {
            return open;
        }

        @Override
        protected void writeBytes(byte[] bytes, int count, boolean hexMode) {
        }

    };

    public Zcode() {
        executor = new ZcodeExecutor(this);
    }

    public void addModule(ZcodeModule m) {
        moduleRegistry.addModule(m);
    }

    public void addChannel(ZcodeChannel ch) {
        ch.setChannelIndex((byte) channels.size());
        channels.add(ch);
        sources.add(ch);
    }

    public void addNotificationSource(ZcodeNotificationSource s) {
        sources.add(s);
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

    public boolean progress() {
        for (ZcodeChannel channel : channels) {
            channel.moveAlong();
        }
        return executor.progress(sources);
    }

    public List<ZcodeChannel> getChannels() {
        return channels;
    }

    public ZcodeModuleRegistry getModuleRegistry() {
        return moduleRegistry;
    }

    public ZcodeOutStream getNotificationOutStream() {
        return notificationOutStream;
    }

    public void setNotificationOutStream(ZcodeOutStream out) {
        this.notificationOutStream = out;
    }

}

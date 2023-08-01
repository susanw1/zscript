package net.zscript.javareceiver.core;

import java.util.ArrayList;
import java.util.List;

import net.zscript.javareceiver.execution.ActionSource;
import net.zscript.javareceiver.execution.ZscriptExecutor;
import net.zscript.javareceiver.modules.ZscriptModule;
import net.zscript.javareceiver.modules.ZscriptModuleRegistry;
import net.zscript.javareceiver.notifications.ZscriptNotificationSource;

public class Zscript {
    private final ZscriptModuleRegistry moduleRegistry = new ZscriptModuleRegistry();
    private final ZscriptLocks          locks          = new ZscriptLocks();
    private final List<ZscriptChannel>  channels       = new ArrayList<>();
    private final List<ActionSource>  sources        = new ArrayList<>();
    private final ZscriptExecutor       executor;

    private OutStream notificationOutStream = new AbstractOutStream() {
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

    public Zscript() {
        executor = new ZscriptExecutor(this);
    }

    public void addModule(ZscriptModule m) {
        moduleRegistry.addModule(m);
    }

    public void addChannel(ZscriptChannel ch) {
        ch.setChannelIndex((byte) channels.size());
        channels.add(ch);
        sources.add(ch);
    }

    public void addNotificationSource(ZscriptNotificationSource s) {
        sources.add(s);
    }

    public boolean lock(LockSet l) {
        return locks.lock(l);
    }

    public boolean canLock(LockSet l) {
        return locks.canLock(l);
    }

    public void unlock(LockSet l) {
        locks.unlock(l);
    }

    public boolean progress() {
        for (ZscriptChannel channel : channels) {
            channel.moveAlong();
        }
        return executor.progress(sources);
    }

    public List<ZscriptChannel> getChannels() {
        return channels;
    }

    public ZscriptModuleRegistry getModuleRegistry() {
        return moduleRegistry;
    }

    public OutStream getNotificationOutStream() {
        return notificationOutStream;
    }

    public void setNotificationOutStream(OutStream out) {
        this.notificationOutStream = out;
    }

}

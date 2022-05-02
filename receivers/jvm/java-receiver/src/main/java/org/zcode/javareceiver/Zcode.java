package org.zcode.javareceiver;

import org.zcode.javareceiver.executionspace.ZcodeExecutionSpace;
import org.zcode.javareceiver.parsing.ZcodeCommandChannel;
import org.zcode.javareceiver.parsing.ZcodeCommandFinder;
import org.zcode.javareceiver.parsing.ZcodeParser;

public class Zcode {
    private final ZcodeNotificationManager notificationManager;
    private final ZcodeExecutionSpace      space;
    private final ZcodeParser              parser;
    private final ZcodeRunner              runner;
    private final ZcodeLocks               zcodeLocks;
    private final ZcodeCommandFinder       finder;
    private final ZcodeDebugOutput         debug;

    private ZcodeCommandChannel[] channels;
    private String                configFailureState = null;

    public Zcode(final ZcodeParameters params, final ZcodeBusInterruptSource[] sources) {
        this.notificationManager = new ZcodeNotificationManager(params, sources);
        this.space = new ZcodeExecutionSpace(params, notificationManager);
        this.parser = new ZcodeParser(this, params);
        this.runner = new ZcodeRunner(this, params);
        this.zcodeLocks = new ZcodeLocks(params);
        this.finder = new ZcodeCommandFinder(params, this);
        this.debug = new ZcodeDebugOutput(params);
    }

    public void configFail(final String configFailureState) {
        this.configFailureState = configFailureState;
    }

    public String getConfigFailureState() {
        return configFailureState;
    }

    public ZcodeExecutionSpace getSpace() {
        return space;
    }

    public void setChannels(final ZcodeCommandChannel[] channels) {
        this.channels = channels;
    }

    public ZcodeCommandFinder getCommandFinder() {
        return finder;
    }

    public ZcodeCommandChannel[] getChannels() {
        return channels;
    }

    public ZcodeDebugOutput getDebug() {
        return debug;
    }

    public boolean canLock(final ZcodeLockSet locks) {
        return zcodeLocks.canLock(locks);
    }

    public void lock(final ZcodeLockSet locks) {
        zcodeLocks.lock(locks);
    }

    public void unlock(final ZcodeLockSet locks) {
        zcodeLocks.unlock(locks);
    }

    public void progressZcode(final int count) {
        for (int i = 0; i < count; i++) {
            progressZcode();
        }
    }

    public void progressZcode() {
        debug.attemptFlush();
        notificationManager.manageNotifications();
        parser.parseNext();
        runner.runNext();
    }

    public ZcodeNotificationManager getNotificationManager() {
        return notificationManager;
    }
}

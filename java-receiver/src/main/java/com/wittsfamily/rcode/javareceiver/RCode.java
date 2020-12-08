package com.wittsfamily.rcode.javareceiver;

import com.wittsfamily.rcode.javareceiver.executionspace.RCodeExecutionSpace;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandFinder;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeParser;

public class RCode {
    private final RCodeNotificationManager notificationManager;
    private final RCodeExecutionSpace space;
    private final RCodeParser parser;
    private final RCodeRunner runner;
    private final RCodeLocks rcodeLocks;
    private final RCodeCommandFinder finder;
    private final RCodeDebugOutput debug;
    private RCodeCommandChannel[] channels;
    private String configFailureState = null;

    public RCode(RCodeParameters params, RCodeBusInterruptSource[] sources) {
        this.notificationManager = new RCodeNotificationManager(params, sources);
        space = new RCodeExecutionSpace(params, notificationManager);
        this.parser = new RCodeParser(this, params);
        this.runner = new RCodeRunner(this, params);
        this.rcodeLocks = new RCodeLocks(params);
        this.finder = new RCodeCommandFinder(params, this);
        this.debug = new RCodeDebugOutput(params);
    }

    public void configFail(String configFailureState) {
        this.configFailureState = configFailureState;
    }

    public String getConfigFailureState() {
        return configFailureState;
    }

    public RCodeExecutionSpace getSpace() {
        return space;
    }

    public void setChannels(RCodeCommandChannel[] channels) {
        this.channels = channels;
    }

    public RCodeCommandFinder getCommandFinder() {
        return finder;
    }

    public RCodeCommandChannel[] getChannels() {
        return channels;
    }

    public RCodeDebugOutput getDebug() {
        return debug;
    }

    public boolean canLock(RCodeLockSet locks) {
        return rcodeLocks.canLock(locks);
    }

    public void lock(RCodeLockSet locks) {
        rcodeLocks.lock(locks);
    }

    public void unlock(RCodeLockSet locks) {
        rcodeLocks.unlock(locks);
    }

    public void progressRCode() {
        debug.attemptFlush();
        notificationManager.manageNotifications();
        parser.parseNext();
        runner.runNext();
    }

    public RCodeNotificationManager getNotificationManager() {
        return notificationManager;
    }
}

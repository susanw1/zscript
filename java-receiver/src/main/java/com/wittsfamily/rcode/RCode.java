package com.wittsfamily.rcode;

import com.wittsfamily.rcode.executionspace.RCodeExecutionSpace;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandFinder;
import com.wittsfamily.rcode.parsing.RCodeParser;

public class RCode {
    private final RCodeNotificationManager notificationManager;
    private final RCodeExecutionSpace space;
    private final RCodeParser parser;
    private final RCodeRunner runner;
    private final RCodeLocks rcodeLocks;
    private final RCodeCommandFinder finder;
    private RCodeCommandChannel[] channels;

    public RCode(RCodeParameters params, RCodeBusInterruptSource[] sources) {
        this.notificationManager = new RCodeNotificationManager(params, sources);
        space = new RCodeExecutionSpace(params, notificationManager);
        this.parser = new RCodeParser(this, params);
        this.runner = new RCodeRunner(this, params);
        this.rcodeLocks = new RCodeLocks(params);
        this.finder = new RCodeCommandFinder(params);
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
        notificationManager.manageNotifications();
        parser.parseNext();
        runner.runNext();
    }

    public RCodeNotificationManager getNotificationManager() {
        return notificationManager;
    }
}

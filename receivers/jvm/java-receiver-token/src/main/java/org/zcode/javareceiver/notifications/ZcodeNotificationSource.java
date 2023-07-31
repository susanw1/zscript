package org.zcode.javareceiver.notifications;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ActionSource;

public class ZcodeNotificationSource implements ActionSource {
    private volatile ZcodeLockSet locks        = null;
    private volatile int          id           = 0;
    private volatile boolean      isAddressing = false;

    @Override
    public ZcodeNotificationAction getAction() {
        return new ZcodeNotificationAction(this);
    }

    public void actionExecuted() {
        id = 0;
    }

    @Override
    public ZcodeOutStream getOutStream(Zcode zcode) {
        return zcode.getNotificationOutStream();
    }

    public int getID() {
        return id;
    }

    public ZcodeLockSet getLocks() {
        return locks;
    }

    public boolean isAddressing() {
        return isAddressing;
    }

    public void setNotification(ZcodeLockSet locks, int notificationID) {
        this.locks = locks;
        this.isAddressing = false;
        this.id = notificationID;
    }

    public void setNotification(int notificationID) {
        this.isAddressing = false;
        this.id = notificationID;
    }

    public void setAddressing(ZcodeLockSet locks, int addressingID) {
        this.locks = locks;
        this.isAddressing = false;
        this.id = addressingID;
    }

    public void setAddressing(int addressingID) {
        this.isAddressing = false;
        this.id = addressingID;
    }

    public void set(ZcodeLockSet locks, int notificationID, boolean isAddressing) {
        this.locks = locks;
        this.isAddressing = isAddressing;
        this.id = notificationID;
    }

    public boolean hasNotification() {
        return id != 0;
    }
}

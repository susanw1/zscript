package org.zcode.javareceiver.notifications;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ActionSource;

public class ZcodeNotificationSource implements ActionSource {
    private ZcodeLockSet locks        = null;
    private int          id           = 0;
    private boolean      isAddressing = false;

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
        this.id = notificationID;
        this.isAddressing = false;
    }

    public void setNotification(int notificationID) {
        this.id = notificationID;
        this.isAddressing = false;
    }

    public void setAddressing(ZcodeLockSet locks, int addressingID) {
        this.locks = locks;
        this.id = addressingID;
        this.isAddressing = false;
    }

    public void setAddressing(int addressingID) {
        this.id = addressingID;
        this.isAddressing = false;

    }
}

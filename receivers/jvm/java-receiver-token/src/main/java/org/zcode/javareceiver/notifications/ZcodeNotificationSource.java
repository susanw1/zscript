package org.zcode.javareceiver.notifications;

import org.zcode.javareceiver.core.Zcode;
import org.zcode.javareceiver.core.ZcodeLockSet;
import org.zcode.javareceiver.core.ZcodeOutStream;
import org.zcode.javareceiver.execution.ActionSource;

public class ZcodeNotificationSource implements ActionSource {
    private ZcodeLockSet locks          = null;
    private int          notificationID = 0;

    @Override
    public ZcodeNotificationAction getAction() {
        return new ZcodeNotificationAction(this);
    }

    public void actionExecuted() {
        notificationID = 0;
    }

    @Override
    public ZcodeOutStream getOutStream(Zcode zcode) {
        return zcode.getNotificationOutStream();
    }

    public int getNotificationID() {
        return notificationID;
    }

    public ZcodeLockSet getLocks() {
        return locks;
    }

    public void setNotification(ZcodeLockSet locks, int notificationID) {
        this.locks = locks;
        this.notificationID = notificationID;
    }

    public void setNotification(int notificationID) {
        this.notificationID = notificationID;
    }
}

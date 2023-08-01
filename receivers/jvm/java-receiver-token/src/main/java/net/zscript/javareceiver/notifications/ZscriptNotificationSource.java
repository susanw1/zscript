package net.zscript.javareceiver.notifications;

import net.zscript.javareceiver.core.Zscript;
import net.zscript.javareceiver.core.LockSet;
import net.zscript.javareceiver.core.OutStream;
import net.zscript.javareceiver.execution.ActionSource;

public class ZscriptNotificationSource implements ActionSource {
    private volatile LockSet locks        = null;
    private volatile int          id           = 0;
    private volatile boolean      isAddressing = false;

    @Override
    public ZscriptNotificationAction getAction() {
        return new ZscriptNotificationAction(this);
    }

    public void actionExecuted() {
        id = 0;
    }

    @Override
    public OutStream getOutStream(Zscript zscript) {
        return zscript.getNotificationOutStream();
    }

    public int getID() {
        return id;
    }

    public LockSet getLocks() {
        return locks;
    }

    public boolean isAddressing() {
        return isAddressing;
    }

    public void setNotification(LockSet locks, int notificationID) {
        this.locks = locks;
        this.isAddressing = false;
        this.id = notificationID;
    }

    public void setNotification(int notificationID) {
        this.isAddressing = false;
        this.id = notificationID;
    }

    public void setAddressing(LockSet locks, int addressingID) {
        this.locks = locks;
        this.isAddressing = false;
        this.id = addressingID;
    }

    public void setAddressing(int addressingID) {
        this.isAddressing = false;
        this.id = addressingID;
    }

    public void set(LockSet locks, int notificationID, boolean isAddressing) {
        this.locks = locks;
        this.isAddressing = isAddressing;
        this.id = notificationID;
    }

    public boolean hasNotification() {
        return id != 0;
    }
}

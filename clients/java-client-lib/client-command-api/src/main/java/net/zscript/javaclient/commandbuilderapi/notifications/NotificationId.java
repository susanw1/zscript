package net.zscript.javaclient.commandbuilderapi.notifications;

public abstract class NotificationId<H extends NotificationHandle<? extends Notification>> {
    public abstract int getNumericId();

    public abstract Class<H> getHandleType();

    public abstract H newHandle();
}

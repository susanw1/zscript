package net.zscript.javaclient.commandbuilder.notifications;

public abstract class NotificationId<T extends NotificationHandle> {
    public abstract int getId();

    public abstract Class<T> getHandleType();

    public abstract T newHandle();
}

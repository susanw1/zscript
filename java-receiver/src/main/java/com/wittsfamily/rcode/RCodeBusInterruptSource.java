package com.wittsfamily.rcode;

public interface RCodeBusInterruptSource {
    boolean hasAddress();

    boolean hasUncheckedNotifications();

    byte takeUncheckedNotificationId();

    byte getNotificationType(byte id);

    byte getNotificationBus(byte id);

    void findAddress(byte id);

    byte getFoundAddress(byte id);

    boolean hasFindableAddress(byte id);

    boolean hasStartedAddressFind(byte id);

    boolean hasFoundAddress(byte id);

    boolean checkFindAddressLocks(byte id);

    void clearNotification(byte id);
}

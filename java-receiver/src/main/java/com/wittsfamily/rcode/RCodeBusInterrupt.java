package com.wittsfamily.rcode;

public class RCodeBusInterrupt {
    private final RCodeBusInterruptSource source;
    private final byte id;

    public RCodeBusInterrupt(RCodeBusInterruptSource source, byte id) {
        this.source = source;
        this.id = id;
    }

    public RCodeBusInterruptSource getSource() {
        return source;
    }

    public byte getId() {
        return id;
    }

    public byte getNotificationType() {
        return source.getNotificationType(id);
    }

    public byte getNotificationBus() {
        return source.getNotificationBus(id);
    }

    public byte getFoundAddress() {
        return source.getFoundAddress(id);
    }

    public boolean hasFindableAddress() {
        return source.hasFindableAddress(id);
    }

    public void findAddress() {
        source.findAddress(id);
    }

    public boolean hasStartedAddressFind() {
        return source.hasStartedAddressFind(id);
    }

    public boolean hasFoundAddress() {
        return source.hasFoundAddress(id);
    }

    public boolean checkFindAddressLocks() {
        return source.checkFindAddressLocks(id);
    }

    public void clear() {
        source.clearNotification(id);
    }
}

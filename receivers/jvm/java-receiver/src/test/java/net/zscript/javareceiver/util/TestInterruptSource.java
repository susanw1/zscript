package net.zscript.javareceiver.util;

import net.zscript.javareceiver.ZcodeBusInterruptSource;

public class TestInterruptSource implements ZcodeBusInterruptSource {
    private byte    notId       = 0;
    private byte    notType     = 0;
    private byte    notBus      = 0;
    private byte    notAddr     = 0;
    private boolean addrFinding = false;

    @Override
    public boolean hasAddress() {
        return true;
    }

    @Override
    public boolean hasUncheckedNotifications() {
        return notId != 0;
    }

    @Override
    public byte takeUncheckedNotificationId() {
        byte id = notId;
        notId = 0;
        return id;
    }

    @Override
    public byte getNotificationType(final byte id) {
        return notType;
    }

    @Override
    public byte getNotificationBus(final byte id) {
        return notBus;
    }

    @Override
    public void findAddress(final byte id) {
        addrFinding = true;
    }

    @Override
    public byte getFoundAddress(final byte id) {
        return notAddr;
    }

    @Override
    public boolean hasFindableAddress(final byte id) {
        return notAddr != 0;
    }

    @Override
    public boolean hasStartedAddressFind(final byte id) {
        return addrFinding;
    }

    @Override
    public boolean hasFoundAddress(final byte id) {
        return addrFinding;
    }

    @Override
    public boolean checkFindAddressLocks(final byte id) {
        return true;
    }

    @Override
    public void clearNotification(final byte id) {
        notId = 0;
        notAddr = 0;
        addrFinding = false;
    }

    public void setNotification(final byte id, final byte notType, final byte notBus, final byte notAddr) {
        this.notId = id;
        this.notType = notType;
        this.notBus = notBus;
        this.notAddr = notAddr;
    }

}

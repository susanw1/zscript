package com.wittsfamily.rcode.test;

import com.wittsfamily.rcode.RCodeBusInterruptSource;

public class TestInterruptSource implements RCodeBusInterruptSource {
    private byte notId = 0;
    private byte notType = 0;
    private byte notBus = 0;
    private byte notAddr = 0;
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
    public byte getNotificationType(byte id) {
        return notType;
    }

    @Override
    public byte getNotificationBus(byte id) {
        return notBus;
    }

    @Override
    public void findAddress(byte id) {
        addrFinding = true;
    }

    @Override
    public byte getFoundAddress(byte id) {
        return notAddr;
    }

    @Override
    public boolean hasFindableAddress(byte id) {
        return notAddr != 0;
    }

    @Override
    public boolean hasStartedAddressFind(byte id) {
        return addrFinding;
    }

    @Override
    public boolean hasFoundAddress(byte id) {
        return addrFinding;
    }

    @Override
    public boolean checkFindAddressLocks(byte id) {
        return true;
    }

    @Override
    public void clearNotification(byte id) {
        notId = 0;
        notAddr = 0;
        addrFinding = false;
    }

    public void setNotification(byte id, byte notType, byte notBus, byte notAddr) {
        this.notId = id;
        this.notType = notType;
        this.notBus = notBus;
        this.notAddr = notAddr;
    }

}

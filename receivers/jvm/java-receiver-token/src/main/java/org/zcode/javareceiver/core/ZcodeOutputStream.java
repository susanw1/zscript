package org.zcode.javareceiver.core;

public class ZcodeOutputStream {
    private boolean lock;

    public boolean lock() {
        if (lock) {
            return false;
        }
        lock = true;
        return true;
    }

    public boolean isLocked() {
        return lock;
    }

    public void unlock() {
        lock = false;
    }

    public void writeStatus(byte status) {

    }

    public void close() {

    }

    public void writeField(byte c, int echoField) {
        // TODO Auto-generated method stub

    }

    public void open() {
        // TODO Auto-generated method stub

    }

    public boolean isOpen() {
        // TODO Auto-generated method stub
        return false;
    }
}

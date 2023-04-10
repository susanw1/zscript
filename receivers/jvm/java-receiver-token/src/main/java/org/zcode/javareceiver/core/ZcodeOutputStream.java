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
}

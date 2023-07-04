package org.zcode.javareceiver.core;

public class ZcodeLocks {
    public static final int LOCK_COUNT = 32;
    private final byte[]    locks      = new byte[LOCK_COUNT];

    public boolean lock(ZcodeLockSet l) {
        if (!canLock(l)) {
            return false;
        }
        byte[] toApply = l.getLocks();
        for (int i = 0; i < toApply.length; i++) {
            locks[i] |= toApply[i];
        }
        return true;
    }

    public boolean canLock(ZcodeLockSet l) {
        byte[] toApply = l.getLocks();
        for (int i = 0; i < toApply.length; i++) {
            if ((locks[i] & toApply[i]) != 0) {
                return false;
            }
        }
        return true;
    }

    public void unlock(ZcodeLockSet l) {
        byte[] toApply = l.getLocks();
        for (int i = 0; i < toApply.length; i++) {
            locks[i] &= ~toApply[i];
        }
    }
}

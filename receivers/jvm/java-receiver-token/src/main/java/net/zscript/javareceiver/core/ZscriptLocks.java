package net.zscript.javareceiver.core;

public class ZscriptLocks {
    public static final int LOCK_BYTENUM = 32;
    private final byte[]    locks      = new byte[LOCK_BYTENUM];

    public boolean lock(LockSet l) {
        if (!canLock(l)) {
            return false;
        }
        byte[] toApply = l.getLocks();
        for (int i = 0; i < toApply.length; i++) {
            locks[i] |= toApply[i];
        }
        return true;
    }

    public boolean canLock(LockSet l) {
        byte[] toApply = l.getLocks();
        for (int i = 0; i < toApply.length; i++) {
            if ((locks[i] & toApply[i]) != 0) {
                return false;
            }
        }
        return true;
    }

    public void unlock(LockSet l) {
        byte[] toApply = l.getLocks();
        for (int i = 0; i < toApply.length; i++) {
            locks[i] &= ~toApply[i];
        }
    }
}

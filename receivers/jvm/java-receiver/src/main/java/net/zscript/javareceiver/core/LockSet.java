package net.zscript.javareceiver.core;

import java.util.Iterator;

public class LockSet {
    private final byte[] locks;

    public static LockSet from(Iterator<Byte> data) {
        byte[] locks = new byte[ZscriptLocks.LOCK_BYTENUM];

        int i = 0;
        for (Iterator<Byte> iterator = data; i < ZscriptLocks.LOCK_BYTENUM && iterator.hasNext();) {
            locks[i++] = iterator.next();
        }
        return new LockSet(locks);
    }

    public static LockSet empty() {
        byte[] locks = new byte[ZscriptLocks.LOCK_BYTENUM];
        for (int i = 0; i < locks.length; i++) {
            locks[i] = (byte) 0;
        }
        return new LockSet(locks);
    }

    public static LockSet allLocked() {
        byte[] locks = new byte[ZscriptLocks.LOCK_BYTENUM];
        for (int i = 0; i < locks.length; i++) {
            locks[i] = (byte) 0xFF;
        }
        return new LockSet(locks);
    }

    private LockSet(byte[] locks) {
        this.locks = locks;
    }

    byte[] getLocks() {
        return locks;
    }

}

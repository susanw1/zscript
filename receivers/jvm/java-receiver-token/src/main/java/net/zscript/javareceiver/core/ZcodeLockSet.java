package net.zscript.javareceiver.core;

import java.util.Iterator;

public class ZcodeLockSet {
    private final byte[] locks;

    public static ZcodeLockSet from(Iterator<Byte> data) {
        byte[] locks = new byte[ZcodeLocks.LOCK_BYTENUM];

        int i = 0;
        for (Iterator<Byte> iterator = data; i < ZcodeLocks.LOCK_BYTENUM && iterator.hasNext();) {
            locks[i++] = iterator.next();
        }
        return new ZcodeLockSet(locks);
    }

    public static ZcodeLockSet empty() {
        byte[] locks = new byte[ZcodeLocks.LOCK_BYTENUM];
        for (int i = 0; i < locks.length; i++) {
            locks[i] = (byte) 0;
        }
        return new ZcodeLockSet(locks);
    }

    public static ZcodeLockSet allLocked() {
        byte[] locks = new byte[ZcodeLocks.LOCK_BYTENUM];
        for (int i = 0; i < locks.length; i++) {
            locks[i] = (byte) 0xFF;
        }
        return new ZcodeLockSet(locks);
    }

    private ZcodeLockSet(byte[] locks) {
        this.locks = locks;
    }

    byte[] getLocks() {
        return locks;
    }

}

package net.zscript.javareceiver.core;

import javax.annotation.Nonnull;
import java.util.Arrays;
import java.util.Iterator;

public class LockSet {
    private final byte[] locks;

    @Nonnull
    public static LockSet from(Iterator<Byte> data) {
        byte[] locks = new byte[ZscriptLocks.LOCK_BYTENUM];

        int i = 0;
        while (i < ZscriptLocks.LOCK_BYTENUM && data.hasNext()) {
            locks[i++] = data.next();
        }
        return new LockSet(locks);
    }

    public static LockSet empty() {
        return new LockSet(new byte[ZscriptLocks.LOCK_BYTENUM]);
    }

    public static LockSet allLocked() {
        byte[] locks = new byte[ZscriptLocks.LOCK_BYTENUM];
        Arrays.fill(locks, (byte) 0xFF);
        return new LockSet(locks);
    }

    private LockSet(byte[] locks) {
        this.locks = locks;
    }

    byte[] getLocks() {
        return locks;
    }

}

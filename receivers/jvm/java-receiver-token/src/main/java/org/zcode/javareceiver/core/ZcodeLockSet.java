package org.zcode.javareceiver.core;

import java.util.Iterator;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeLockSet {
    private final byte[] locks;

    public static ZcodeLockSet from(ReadToken first) {
        byte[] locks = new byte[first.getDataSize()];

        int i = 0;
        for (Iterator<Byte> iterator = first.blockIterator(); iterator.hasNext();) {
            locks[i++] = iterator.next();
        }
        return new ZcodeLockSet(locks);
    }

    public static ZcodeLockSet allLocked() {
        byte[] locks = new byte[ZcodeLocks.LOCK_COUNT];
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

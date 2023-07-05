package org.zcode.javareceiver.core;

import static org.assertj.core.api.Assertions.assertThat;

import java.util.Iterator;
import java.util.Random;

import org.junit.jupiter.api.Test;

public class ZcodeLocksTest {
    private class ArrayIter implements Iterator<Byte> {
        private final byte[] expected;
        private int          i = 0;

        public ArrayIter(byte[] expected) {
            this.expected = expected;
        }

        @Override
        public Byte next() {
            return expected[i++];
        }

        @Override
        public boolean hasNext() {
            return i < expected.length;
        }
    }

    @Test
    void lockSetShouldInitialiseAsLocked() {
        ZcodeLockSet locks    = ZcodeLockSet.allLocked();
        byte[]       expected = new byte[ZcodeLocks.LOCK_BYTENUM];
        for (int i = 0; i < expected.length; i++) {
            expected[i] = (byte) 0xFF;
        }
        assertThat(locks.getLocks()).containsExactly(expected);
    }

    @Test
    void lockSetShouldInitialiseFromIterator() {
        Random r = new Random(12345);
        for (int i = 0; i < 20; i++) {
            byte[] expected = new byte[ZcodeLocks.LOCK_BYTENUM];
            r.nextBytes(expected);
            ZcodeLockSet locks = ZcodeLockSet.from(new ArrayIter(expected));
            assertThat(locks.getLocks()).containsExactly(expected);

        }
    }

    @Test
    void locksShouldDetectConflict() {
        byte[]     data1 = new byte[] { 0x10, 0x0f };
        byte[]     data2 = new byte[] { 0x70, 0x01 };
        byte[]     data3 = new byte[] { (byte) 0xef, (byte) 0xf0 };
        ZcodeLocks locks = new ZcodeLocks();
        assertThat(locks.canLock(ZcodeLockSet.from(new ArrayIter(data1)))).isTrue();
        assertThat(locks.lock(ZcodeLockSet.from(new ArrayIter(data1)))).isTrue();
        assertThat(locks.canLock(ZcodeLockSet.from(new ArrayIter(data2)))).isFalse();
        assertThat(locks.lock(ZcodeLockSet.from(new ArrayIter(data2)))).isFalse();
        assertThat(locks.canLock(ZcodeLockSet.from(new ArrayIter(data3)))).isTrue();
        assertThat(locks.lock(ZcodeLockSet.from(new ArrayIter(data3)))).isTrue();
    }

    @Test
    void shouldUnlock() {
        byte[]     data1 = new byte[] { 0x10, 0x0f };
        byte[]     data2 = new byte[] { 0x70, 0x01 };
        ZcodeLocks locks = new ZcodeLocks();
        assertThat(locks.lock(ZcodeLockSet.from(new ArrayIter(data1)))).isTrue();
        assertThat(locks.lock(ZcodeLockSet.from(new ArrayIter(data2)))).isFalse();
        locks.unlock(ZcodeLockSet.from(new ArrayIter(data1)));
        assertThat(locks.lock(ZcodeLockSet.from(new ArrayIter(data2)))).isTrue();
    }

}

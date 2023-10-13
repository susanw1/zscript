package net.zscript.javareceiver.core;

import static org.assertj.core.api.Assertions.assertThat;

import java.util.Iterator;
import java.util.Random;

import org.junit.jupiter.api.Test;

public class ZscriptLocksTest {
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
        LockSet locks    = LockSet.allLocked();
        byte[]  expected = new byte[ZscriptLocks.LOCK_BYTENUM];
        for (int i = 0; i < expected.length; i++) {
            expected[i] = (byte) 0xFF;
        }
        assertThat(locks.getLocks()).containsExactly(expected);
    }

    @Test
    void lockSetShouldInitialiseFromIterator() {
        Random r = new Random(12345);
        for (int i = 0; i < 20; i++) {
            byte[] expected = new byte[ZscriptLocks.LOCK_BYTENUM];
            r.nextBytes(expected);
            LockSet locks = LockSet.from(new ArrayIter(expected));
            assertThat(locks.getLocks()).containsExactly(expected);

        }
    }

    @Test
    void locksShouldDetectConflict() {
        byte[]       data1 = new byte[] { 0x10, 0x0f };
        byte[]       data2 = new byte[] { 0x70, 0x01 };
        byte[]       data3 = new byte[] { (byte) 0xef, (byte) 0xf0 };
        ZscriptLocks locks = new ZscriptLocks();
        assertThat(locks.canLock(LockSet.from(new ArrayIter(data1)))).isTrue();
        assertThat(locks.lock(LockSet.from(new ArrayIter(data1)))).isTrue();
        assertThat(locks.canLock(LockSet.from(new ArrayIter(data2)))).isFalse();
        assertThat(locks.lock(LockSet.from(new ArrayIter(data2)))).isFalse();
        assertThat(locks.canLock(LockSet.from(new ArrayIter(data3)))).isTrue();
        assertThat(locks.lock(LockSet.from(new ArrayIter(data3)))).isTrue();
    }

    @Test
    void shouldUnlock() {
        byte[]       data1 = new byte[] { 0x10, 0x0f };
        byte[]       data2 = new byte[] { 0x70, 0x01 };
        ZscriptLocks locks = new ZscriptLocks();
        assertThat(locks.lock(LockSet.from(new ArrayIter(data1)))).isTrue();
        assertThat(locks.lock(LockSet.from(new ArrayIter(data2)))).isFalse();
        locks.unlock(LockSet.from(new ArrayIter(data1)));
        assertThat(locks.lock(LockSet.from(new ArrayIter(data2)))).isTrue();
    }

}

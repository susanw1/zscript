package org.zcode.javaclient.parsing;

import java.util.BitSet;

public class LockSet {
    private BitSet bitSet;

    LockSet() {
        bitSet = new BitSet();
    }

    public static LockSetBuilder builder() {
        return new LockSetBuilder();
    }

    public static class LockSetBuilder {
        public LockSet build() {
            return new LockSet();
        }

    }
}

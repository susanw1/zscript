package net.zscript.javaclient.sequence;

import java.util.BitSet;

import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

public class LockSet implements ByteAppendable {
    private final BitSet  locks;
    private final boolean supports32;

    public static LockSet parse(ReadToken token, boolean supports32) {
        if (token.getKey() != Zchars.Z_LOCKS) {
            throw new IllegalArgumentException("Cannot parse lock set from non-lock token");
        }
        if (token.getDataSize() > 4 || (!supports32 && token.getDataSize() > 2)) {
            throw new IllegalArgumentException("Lock field too long");
        }
        return new LockSet(supports32, token.getData32());
    }

    public static LockSet allLocked(boolean supports32) {
        return new LockSet(supports32, supports32 ? 0xFFFFFFFFL : 0xFFFF);
    }

    public static LockSet noneLocked(boolean supports32) {
        return new LockSet(supports32, 0);
    }

    private LockSet(boolean supports32, long data) {
        this.locks = new BitSet(supports32 ? 32 : 16);
        locks.or(BitSet.valueOf(new long[] { data }));
        this.supports32 = supports32;
    }

    public void setLock(int lock) {
        if (lock >= 32 || (!supports32 && lock >= 16)) {
            throw new IllegalArgumentException("Lock out of range for device: " + lock);
        }
        locks.set(lock);
    }

    public void combineWith(LockSet other) {
        locks.or(other.locks);
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        if (isAllSet()) {
            return;
        }
        final long data = locks.isEmpty() ? 0 : locks.toLongArray()[0];
        builder.appendByte(Zchars.Z_LOCKS).appendNumeric32(data);
    }

    @Override
    public String toString() {
        return toStringImpl();
    }

    public boolean isAllSet() {
        return locks.nextClearBit(0) >= (supports32 ? 32 : 16);
    }

    public int getBufferLength() {
        if (isAllSet()) {
            return 0;
        }
        final long value = locks.isEmpty() ? 0 : locks.toLongArray()[0];
        if (value > 0xFFFFFF) {
            return 6;
        } else if (value > 0xFFFF) {
            return 5;
        } else if (value > 0xFF) {
            return 4;
        } else if (value > 0) {
            return 3;
        } else {
            return 2;
        }
    }
}

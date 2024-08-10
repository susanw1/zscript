package net.zscript.javaclient.sequence;

import java.util.BitSet;

import static net.zscript.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.model.components.Zchars;
import net.zscript.util.ByteString.ByteAppendable;
import net.zscript.util.ByteString.ByteStringBuilder;

public class ZscriptLockSet implements ByteAppendable {
    private final BitSet  locks;
    private final boolean supports32;

    public static ZscriptLockSet parse(ReadToken token, boolean supports32) {
        if (token.getKey() != Zchars.Z_LOCKS) {
            throw new IllegalArgumentException("Cannot parse lock set from non-lock token");
        }
        if (token.getDataSize() > 4 || (!supports32 && token.getDataSize() > 2)) {
            throw new IllegalArgumentException("Lock field too long");
        }
        return new ZscriptLockSet(supports32, token.getData32());
    }

    public static ZscriptLockSet allLocked(boolean supports32) {
        return new ZscriptLockSet(true, supports32 ? 0xFFFFFFFFL : 0xFFFF);
    }

    public static ZscriptLockSet noneLocked(boolean supports32) {
        return new ZscriptLockSet(supports32, 0);
    }

    private ZscriptLockSet(boolean supports32, long data) {
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

    public void combineWith(ZscriptLockSet other) {
        locks.or(other.locks);
    }

    @Override
    public void appendTo(ByteStringBuilder builder) {
        if (isAllSet()) {
            return;
        }
        long data = locks.toLongArray()[0];
        builder.appendByte(Zchars.Z_LOCKS).appendNumeric32(data);
    }

    public boolean isAllSet() {
        return locks.nextClearBit(0) >= (supports32 ? 32 : 16);
    }

    public int getBufferLength() {
        if (isAllSet()) {
            return 0;
        }
        long value = locks.toLongArray()[0];
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

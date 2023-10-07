package net.zscript.javaclient.sequence;

import java.util.BitSet;

import static net.zscript.javareceiver.tokenizer.TokenBuffer.TokenReader.ReadToken;

import net.zscript.javaclient.commandbuilder.ZscriptByteString;
import net.zscript.model.components.Zchars;

public class ZscriptLockSet {
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
        if (supports32) {
            return new ZscriptLockSet(true, 0xFFFFFFFFL);
        } else {
            return new ZscriptLockSet(false, 0xFFFF);
        }
    }

    public static ZscriptLockSet noneLocked(boolean supports32) {
        if (supports32) {
            return new ZscriptLockSet(true, 0);
        } else {
            return new ZscriptLockSet(false, 0);
        }
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

    public void toBytes(ZscriptByteString.ZscriptByteStringBuilder builder) {
        if (locks.nextClearBit(0) >= (supports32 ? 32 : 16)) {
            return;
        }
        long data = locks.toLongArray()[0];
        builder.appendField32(Zchars.Z_LOCKS, data);

    }
}

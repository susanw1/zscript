package org.zcode.javareceiver.core;

import org.zcode.javareceiver.tokenizer.Zchars;
import org.zcode.javareceiver.tokenizer.ZcodeTokenDataIterator;

public class ZcodeLockSet {

    public void set(ZcodeTokenDataIterator data) {
        if (data.getKey() != Zchars.Z_LOCKS) {
            throw new IllegalArgumentException("Zcode Locks must be parsed from a lock token");
        }
    }

    public void setAll() {

    }
}

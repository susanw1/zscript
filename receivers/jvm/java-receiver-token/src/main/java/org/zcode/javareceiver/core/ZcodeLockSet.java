package org.zcode.javareceiver.core;

import java.util.Iterator;

import org.zcode.javareceiver.tokenizer.ZcodeTokenBuffer.TokenReader.ReadToken;

public class ZcodeLockSet {

    public static ZcodeLockSet from(ReadToken first) {
        return new ZcodeLockSet();
    }

    public static ZcodeLockSet allLocked() {
        return new ZcodeLockSet();
    }

    private ZcodeLockSet() {
        // TODO Auto-generated constructor stub
    }

    public void set(Iterator<Byte> data) {
    }

    public void setAll() {

    }
}

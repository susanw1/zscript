package org.zcode.javareceiver.core;

import org.zcode.javareceiver.addressing.AddressingResult;
import org.zcode.javareceiver.tokenizer.ZcodeTokenIterator;

public class Zcode {
    public boolean lock(ZcodeLockSet l) {
        return true;
    }

    public boolean canLock(ZcodeLockSet l) {
        return true;
    }

    public void unlock(ZcodeLockSet l) {
    }

    public void address(ZcodeTokenIterator commandToAddress, AddressingResult resultLoc) {

    }

    public void deviceFatalError() {

    }

}

package com.wittsfamily.rcode.javareceiver.commands;

import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public interface RCodeCommand {

    default void finish(RCodeCommandSlot rCodeCommandSlot, RCodeOutStream out) {
    }

    void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out);

    void setLocks(RCodeLockSet locks);

    byte getCode();

    byte[] getFullCode();

    boolean matchesCode(byte[] code);

    int getCodeLength();

    default void moveAlong(RCodeCommandSlot slot) {
    }
}

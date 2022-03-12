package org.zcode.javareceiver.commands;

import org.zcode.javareceiver.ZcodeLockSet;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;
import org.zcode.javareceiver.parsing.ZcodeCommandSlot;

public interface ZcodeCommand {

    default void finish(final ZcodeCommandSlot zcodeCommandSlot, final ZcodeOutStream out) {
    }

    void execute(ZcodeCommandSlot slot, ZcodeCommandSequence sequence, ZcodeOutStream out);

    void setLocks(ZcodeLockSet locks);

    byte getCode();

    byte[] getFullCode();

    boolean matchesCode(byte[] code);

    int getCodeLength();

    default void moveAlong(final ZcodeCommandSlot slot) {
    }
}

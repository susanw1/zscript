package net.zscript.javareceiver.commands;

import net.zscript.javareceiver.ZcodeLockSet;
import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.parsing.ZcodeCommandSequence;
import net.zscript.javareceiver.parsing.ZcodeCommandSlot;

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

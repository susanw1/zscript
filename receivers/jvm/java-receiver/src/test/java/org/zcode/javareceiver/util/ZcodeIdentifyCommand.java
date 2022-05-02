package org.zcode.javareceiver.util;

import org.zcode.javareceiver.ZcodeLockSet;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeResponseStatus;
import org.zcode.javareceiver.commands.ZcodeCommand;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;
import org.zcode.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeIdentifyCommand implements ZcodeCommand {

    @Override
    public void execute(final ZcodeCommandSlot slot, final ZcodeCommandSequence sequence, final ZcodeOutStream out) {
        out.writeField('V', (byte) 0)
                .continueField((byte) 0)
                .continueField((byte) 1)
                .writeBigStringField("Java/Test")
                .writeStatus(ZcodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void finish(final ZcodeCommandSlot slot, final ZcodeOutStream out) {
    }

    @Override
    public void setLocks(final ZcodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 0;
    }

    @Override
    public boolean matchesCode(final byte[] code) {
        return code.length == 1 && code[0] == getCode();
    }

    @Override
    public int getCodeLength() {
        return 1;
    }

    @Override
    public byte[] getFullCode() {
        return new byte[] { getCode() };
    }

}

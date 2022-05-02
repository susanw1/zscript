package org.zcode.javareceiver.commands;

import org.zcode.javareceiver.ZcodeLockSet;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeResponseStatus;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;
import org.zcode.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeActivateCommand implements ZcodeCommand {
    public static final int MAX_SYSTEM_CODE = 15;
    private static boolean  isActivated     = false;

    public static boolean isActivated() {
        return isActivated;
    }

    public static void reset() {
        isActivated = false;
    }

    @Override
    public void execute(final ZcodeCommandSlot slot, final ZcodeCommandSequence sequence, final ZcodeOutStream out) {
        if (!isActivated) {
            out.writeField('A', (byte) 0);
            out.writeStatus(ZcodeResponseStatus.OK);
        } else {
            out.writeField('A', (byte) 1);
            out.writeStatus(ZcodeResponseStatus.OK);
        }
        isActivated = true;
        slot.setComplete(true);
    }

    @Override
    public void setLocks(final ZcodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 3;
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

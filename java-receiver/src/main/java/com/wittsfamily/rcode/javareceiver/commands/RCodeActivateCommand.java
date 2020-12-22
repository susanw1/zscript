package com.wittsfamily.rcode.javareceiver.commands;

import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public class RCodeActivateCommand implements RCodeCommand {
    public static final int MAX_SYSTEM_CODE = 15;
    private static boolean isActivated = false;

    public static boolean isActivated() {
        return isActivated;
    }

    public static void reset() {
        isActivated = false;
    }

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        if (!isActivated) {
            out.writeField('A', (byte) 0);
            out.writeStatus(RCodeResponseStatus.OK);
        } else {
            out.writeField('A', (byte) 1);
            out.writeStatus(RCodeResponseStatus.OK);
        }
        isActivated = true;
        slot.setComplete(true);
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 3;
    }

    @Override
    public boolean matchesCode(byte[] code) {
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

package com.wittsfamily.rcode.commands;

import com.wittsfamily.rcode.RCodeLockSet;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeResponseStatus;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeCommandSlot;

public class RCodeActivateCommand implements RCodeCommand {
    public static final int MAX_SYSTEM_CODE = 15;
    private static boolean isActivated = false;
    private static RCodeCommandSlot changed = null;

    public static boolean isActivated() {
        return isActivated;
    }

    @Override
    public boolean continueLocking(RCodeCommandChannel c) {
        return true;
    }

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        if (!isActivated) {
            changed = slot;
        }
        isActivated = true;
    }

    @Override
    public void finish(RCodeCommandSlot slot, RCodeOutStream out) {
        if (slot == changed) {
            out.writeField('A', (byte) 0);
            out.writeStatus(RCodeResponseStatus.OK);
        } else {
            out.writeField('A', (byte) 1);
            out.writeStatus(RCodeResponseStatus.OK);
        }
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 6;
    }

}

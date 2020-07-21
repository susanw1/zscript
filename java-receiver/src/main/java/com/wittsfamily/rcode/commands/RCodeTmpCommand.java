package com.wittsfamily.rcode.commands;

import com.wittsfamily.rcode.RCodeLockSet;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeResponseStatus;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeCommandSlot;

public class RCodeTmpCommand implements RCodeCommand {
    @Override
    public boolean continueLocking(RCodeCommandChannel c) {
        return true;
    }

    @Override
    public void finish(RCodeCommandSlot rCodeCommandSlot, RCodeOutStream out) {
    }

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        if (!slot.getFields().has('S')) {
            out.writeStatus(RCodeResponseStatus.OK);
        }
        slot.getFields().copyTo(out);
        slot.getBigField().copyTo(out);
        slot.setComplete(true);
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 0x10;
    }

}

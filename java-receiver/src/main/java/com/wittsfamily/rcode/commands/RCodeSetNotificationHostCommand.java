package com.wittsfamily.rcode.commands;

import com.wittsfamily.rcode.RCode;
import com.wittsfamily.rcode.RCodeLockSet;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.RCodeResponseStatus;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeCommandSlot;

public class RCodeSetNotificationHostCommand implements RCodeCommand {
    private final RCode rcode;

    public RCodeSetNotificationHostCommand(RCode rcode) {
        this.rcode = rcode;
    }

    @Override
    public boolean continueLocking(RCodeCommandChannel c) {
        return true;
    }

    @Override
    public void finish(RCodeCommandSlot rCodeCommandSlot, RCodeOutStream out) {
    }

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        rcode.getNotificationManager().setNotificationChannel(sequence.getChannel(), sequence.getNotificationAddress());
        out.writeStatus(RCodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 8;
    }

}

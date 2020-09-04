package com.wittsfamily.rcode.javareceiver.commands;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public class RCodeSetDebugChannelCommand implements RCodeCommand {
    private final RCode rcode;

    public RCodeSetDebugChannelCommand(RCode rcode) {
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
        rcode.getDebug().setDebugChannel(sequence.getChannel());
        rcode.getDebug().println("Debug channel set...");
        out.writeStatus(RCodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 9;
    }

    @Override
    public boolean matchesCode(byte[] code) {
        return code.length == 1 && code[0] == getCode();
    }

    @Override
    public int getCodeLength() {
        return 1;
    }
}

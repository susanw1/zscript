package com.wittsfamily.rcode.javareceiver.commands;

import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.executionspace.RCodeExecutionSpace;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public class RCodeExecutionSpaceCommand implements RCodeCommand {
    private final RCodeExecutionSpace space;

    public RCodeExecutionSpaceCommand(RCodeExecutionSpace space) {
        this.space = space;
    }

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        if (slot.getFields().has('G')) {
            space.setRunning(true);
        } else {
            space.setRunning(false);
        }
        out.writeStatus(RCodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 0x21;
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

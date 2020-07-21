package com.wittsfamily.rcode.commands;

import com.wittsfamily.rcode.RCodeLockSet;
import com.wittsfamily.rcode.RCodeOutStream;
import com.wittsfamily.rcode.executionspace.RCodeExecutionSpace;
import com.wittsfamily.rcode.parsing.RCodeCommandChannel;
import com.wittsfamily.rcode.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.parsing.RCodeCommandSlot;

public class RCodeExecutionSpaceCommand implements RCodeCommand {
    private final RCodeExecutionSpace space;

    private RCodeExecutionSpaceCommand(RCodeExecutionSpace space) {
        this.space = space;
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
        if (slot.getFields().has('G')) {
            space.setRunning(true);
        } else {
            space.setRunning(false);
        }
        slot.setComplete(true);
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 0x21;
    }

}

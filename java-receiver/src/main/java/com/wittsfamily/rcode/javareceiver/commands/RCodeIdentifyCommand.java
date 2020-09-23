package com.wittsfamily.rcode.javareceiver.commands;

import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public class RCodeIdentifyCommand implements RCodeCommand {

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        out.writeField('V', (byte) 1);
        out.continueField((byte) 3);
        out.continueField((byte) 1);
        out.writeBigStringField("Java/Test");
        out.writeStatus(RCodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void finish(RCodeCommandSlot slot, RCodeOutStream out) {
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 0;
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

package com.wittsfamily.rcode.javareceiver.commands;

import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public class RCodeEchoCommand implements RCodeCommand {

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        if (!slot.getFields().has('S')) {
            out.writeStatus(RCodeResponseStatus.OK);
        } else {
            RCodeResponseStatus stat = RCodeResponseStatus.OK;
            byte val = slot.getFields().get('S', (byte) 0);
            for (RCodeResponseStatus r : RCodeResponseStatus.values()) {
                if (r.getValue() == val) {
                    stat = r;
                    break;
                }
            }
            slot.fail("", stat);
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
        return 1;
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

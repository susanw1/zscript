package com.wittsfamily.rcode.javareceiver;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

import com.wittsfamily.rcode.javareceiver.RCode;
import com.wittsfamily.rcode.javareceiver.RCodeLockSet;
import com.wittsfamily.rcode.javareceiver.RCodeOutStream;
import com.wittsfamily.rcode.javareceiver.RCodeResponseStatus;
import com.wittsfamily.rcode.javareceiver.commands.RCodeCommand;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSequence;
import com.wittsfamily.rcode.javareceiver.parsing.RCodeCommandSlot;

public class RCodeWriteToDebugCommand implements RCodeCommand {
    private final RCode rcode;

    public RCodeWriteToDebugCommand(RCode rcode) {
        this.rcode = rcode;
    }

    @Override
    public void finish(RCodeCommandSlot rCodeCommandSlot, RCodeOutStream out) {
    }

    @Override
    public void execute(RCodeCommandSlot slot, RCodeCommandSequence sequence, RCodeOutStream out) {
        rcode.getDebug().println(StandardCharsets.UTF_8.decode(ByteBuffer.wrap(slot.getBigField().getData(), 0, slot.getBigField().getLength())).toString());
        out.writeStatus(RCodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void setLocks(RCodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 0x60;
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

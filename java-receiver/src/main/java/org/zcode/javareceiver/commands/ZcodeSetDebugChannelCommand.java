package org.zcode.javareceiver.commands;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeLockSet;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeResponseStatus;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;
import org.zcode.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeSetDebugChannelCommand implements ZcodeCommand {
    private final Zcode zcode;

    public ZcodeSetDebugChannelCommand(final Zcode zcode) {
        this.zcode = zcode;
    }

    @Override
    public void execute(final ZcodeCommandSlot slot, final ZcodeCommandSequence sequence, final ZcodeOutStream out) {
        zcode.getDebug().setDebugChannel(sequence.getChannel());
        zcode.getDebug().println("Debug channel set...");
        out.writeStatus(ZcodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void setLocks(final ZcodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 9;
    }

    @Override
    public boolean matchesCode(final byte[] code) {
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

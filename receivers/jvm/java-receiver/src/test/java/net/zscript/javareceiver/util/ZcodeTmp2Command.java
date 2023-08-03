package net.zscript.javareceiver.util;

import net.zscript.javareceiver.ZcodeLockSet;
import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.ZcodeResponseStatus;
import net.zscript.javareceiver.commands.ZcodeCommand;
import net.zscript.javareceiver.parsing.ZcodeCommandSequence;
import net.zscript.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeTmp2Command implements ZcodeCommand {
    @Override
    public void finish(final ZcodeCommandSlot zcodeCommandSlot, final ZcodeOutStream out) {
    }

    @Override
    public void execute(final ZcodeCommandSlot slot, final ZcodeCommandSequence sequence, final ZcodeOutStream out) {
        out.writeStatus(ZcodeResponseStatus.CMD_FAIL);
        slot.getFields().copyTo(out);
        slot.getBigField().copyTo(out);
        slot.fail("", ZcodeResponseStatus.CMD_FAIL);
        slot.setComplete(true);
    }

    @Override
    public void setLocks(final ZcodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 0x11;
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
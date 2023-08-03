package net.zscript.javareceiver.commands;

import net.zscript.javareceiver.Zchars;
import net.zscript.javareceiver.ZcodeLockSet;
import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.ZcodeResponseStatus;
import net.zscript.javareceiver.parsing.ZcodeCommandSequence;
import net.zscript.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeEchoCommand implements ZcodeCommand {

    @Override
    public void execute(final ZcodeCommandSlot slot, final ZcodeCommandSequence sequence, final ZcodeOutStream out) {
        if (!slot.getFields().has(Zchars.STATUS_RESP_PARAM.ch)) {
            out.writeStatus(ZcodeResponseStatus.OK);
        } else {
            ZcodeResponseStatus stat = ZcodeResponseStatus.OK;
            final byte          val  = slot.getFields().get(Zchars.STATUS_RESP_PARAM.ch, (byte) 0);
            for (final ZcodeResponseStatus r : ZcodeResponseStatus.values()) {
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
    public void setLocks(final ZcodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 1;
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
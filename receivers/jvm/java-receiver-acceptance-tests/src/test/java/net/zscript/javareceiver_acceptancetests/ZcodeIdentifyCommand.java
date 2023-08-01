package net.zscript.javareceiver_acceptancetests;

import net.zscript.javareceiver.ZcodeLockSet;
import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.ZcodeResponseStatus;
import net.zscript.javareceiver.commands.ZcodeCommand;
import net.zscript.javareceiver.parsing.ZcodeCommandSequence;
import net.zscript.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeIdentifyCommand implements ZcodeCommand {

    @Override
    public void execute(ZcodeCommandSlot slot, ZcodeCommandSequence sequence, ZcodeOutStream out) {
        out.writeField('V', (byte) 0);
        out.continueField((byte) 0);
        out.continueField((byte) 1);
        out.writeBigStringField("Java/AcceptanceTests");
        out.writeStatus(ZcodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void finish(ZcodeCommandSlot slot, ZcodeOutStream out) {
    }

    @Override
    public void setLocks(ZcodeLockSet locks) {
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

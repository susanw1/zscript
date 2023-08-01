package net.zscript.javareceiver.util;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

import net.zscript.javareceiver.Zcode;
import net.zscript.javareceiver.ZcodeLockSet;
import net.zscript.javareceiver.ZcodeOutStream;
import net.zscript.javareceiver.ZcodeResponseStatus;
import net.zscript.javareceiver.commands.ZcodeCommand;
import net.zscript.javareceiver.parsing.ZcodeCommandSequence;
import net.zscript.javareceiver.parsing.ZcodeCommandSlot;

public class ZcodeWriteToDebugCommand implements ZcodeCommand {
    private final Zcode zcode;

    public ZcodeWriteToDebugCommand(final Zcode zcode) {
        this.zcode = zcode;
    }

    @Override
    public void finish(final ZcodeCommandSlot zcodeCommandSlot, final ZcodeOutStream out) {
    }

    @Override
    public void execute(final ZcodeCommandSlot slot, final ZcodeCommandSequence sequence, final ZcodeOutStream out) {
        zcode.getDebug().println(StandardCharsets.UTF_8.decode(ByteBuffer.wrap(slot.getBigField().getData(), 0, slot.getBigField().getLength())).toString());
        out.writeStatus(ZcodeResponseStatus.OK);
        slot.setComplete(true);
    }

    @Override
    public void setLocks(final ZcodeLockSet locks) {
    }

    @Override
    public byte getCode() {
        return 0x60;
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

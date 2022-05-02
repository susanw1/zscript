package org.zcode.javareceiver.util;

import java.nio.ByteBuffer;
import java.nio.charset.StandardCharsets;

import org.zcode.javareceiver.Zcode;
import org.zcode.javareceiver.ZcodeLockSet;
import org.zcode.javareceiver.ZcodeOutStream;
import org.zcode.javareceiver.ZcodeResponseStatus;
import org.zcode.javareceiver.commands.ZcodeCommand;
import org.zcode.javareceiver.parsing.ZcodeCommandSequence;
import org.zcode.javareceiver.parsing.ZcodeCommandSlot;

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
